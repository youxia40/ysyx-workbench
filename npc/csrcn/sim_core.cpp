#include "sim_core.h"
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <dlfcn.h>
#include <cstring>

static SimContext* global_sim_context = nullptr;

extern "C" SimContext* get_sim_context() {
    return global_sim_context;
}

static SimContext* global_simulation_context = nullptr;


void print_stack_trace(SimContext* sim) {
    if (!sim) {
        printf("[STACK] No simulation context\n");
        return;
    }
    
    // 使用调用约定确定栈帧信息（RV32E简单实现）
    uint32_t ra = sim->regs[1];  // x1 (ra)
    uint32_t sp = sim->regs[2];  // x2 (sp)
    
    printf("\n[STACK TRACE]\n");
    printf("Current PC: 0x%08x\n", sim->pc);
    printf("Return Address (ra): 0x%08x\n", ra);
    
    // 简单堆栈追溯
    uint32_t frame_ptr = sp;
    for (int i = 0; i < 16 && frame_ptr; i++) {
        // 获取上一个返回地址（假设堆栈格式：ra值在sp位置）
        uint32_t prev_ra = pmem_read(frame_ptr);
        
        // 边界检查
        if (prev_ra >= PMEM_BASE && prev_ra <= PMEM_END) {
            printf("#%d Frame Ptr: 0x%08x | Return Addr: 0x%08x\n", 
                   i, frame_ptr, prev_ra);
            frame_ptr += 4; // 移动到下一个堆栈帧
        } else {
            printf("End of stack trace (invalid addr: 0x%08x)\n", prev_ra);
            break;
        }
    }
}

void record_execution(SimContext* sim, uint32_t pc, uint32_t inst) {
    if (!sim) return;
    
    // 解决编译错误：显式类型转换
    sim->history[sim->history_idx] = (ExecutionRecord) {
        .pc = pc,
        .inst = inst,
        .opcode = static_cast<uint8_t>(inst & 0x7F) // 显式转换解决窄化问题
    };
    
    sim->history_idx = (sim->history_idx + 1) % EXEC_HISTORY_SIZE;
}

void print_execution_history(SimContext* sim) {
    if (!sim) return;

    fprintf(stderr, "\n=== LAST 10 EXECUTED INSTRUCTIONS ===\n");
    for (int i = 0; i < EXEC_HISTORY_SIZE; i++) {
        int idx = (sim->history_idx + i) % EXEC_HISTORY_SIZE;
        ExecutionRecord* r = &sim->history[idx];
        
        if (r->pc == 0 && r->inst == 0) continue; // 跳过空记录
        
        fprintf(stderr, "Instr#%d: PC=0x%08x | INST=0x%08x | OP=0x%02x\n",
                idx, r->pc, r->inst, r->opcode);
    }
}

void safe_set_pc(SimContext* sim, uint32_t new_pc) {
    if (!sim) return;
    
    // 记录修改尝试
    fprintf(stderr, "[PC_UPDATE] Attempt: Current=0x%08x → New=0x%08x\n", 
            sim->pc, new_pc);
    
    // 严格边界检查
    if (new_pc < PMEM_BASE || new_pc > PMEM_END) {
        fprintf(stderr, "\n[CRITICAL] INVALID PC UPDATE DETECTED!\n");
        fprintf(stderr, "  Current PC: 0x%08x\n", sim->pc);
        fprintf(stderr, "  Attempting to set: 0x%08x\n", new_pc);
        
        // 打印最后10条执行的指令
        print_execution_history(sim);
        
        // 强制停止模拟
        sim->stop = true;
        return;
    }
    
    // 原子更新
    sim->last_valid_pc = sim->pc;  // 保存旧值
    sim->pc = new_pc;              // 更新新值
    sim->pc_modification_count++;  // 增加计数器
    
    // 记录成功更新
    fprintf(stderr, "[PC_UPDATE] Success: PC=0x%08x (Modifier: %u, Count: %u)\n", 
            sim->pc, sim->pc_modifier, sim->pc_modification_count);
}


extern "C" void enable_diff_test(SimContext* sim, bool enable) {
    sim->diff_test_enabled = enable;
    if (enable && !sim->nemu_handle) {
        // 加载NEMU共享库
        const char* nemu_lib_path = getenv("NEMU_SHARED_LIB");
        if (!nemu_lib_path) {
            nemu_lib_path = "/home/pz40/ysyx-workbench/nemu/build/riscv32-nemu-interpreter-so";
        }
        
        sim->nemu_handle = dlopen(nemu_lib_path, RTLD_LAZY);
        if (!sim->nemu_handle) {
            fprintf(stderr, "Failed to load NEMU: %s\n", dlerror());
            sim->diff_test_enabled = false;
        }
    }
}

extern "C" SimContext* sim_init() {
    SimContext* sim = new SimContext();
    if (!sim) return nullptr;
    
    // 内存初始化
    sim->mem_size = 128 * 1024 * 1024;  // 128MB
    sim->memory = new uint8_t[sim->mem_size]();
    
    // 寄存器初始化
    memset(sim->regs, 0, sizeof(sim->regs));
    
    // PC 初始化
    sim->pc = PMEM_BASE;
    
    // 其他字段初始化
    sim->total_instructions = 0;
    sim->stop = false;
    sim->hit_good_trap = false;
    sim->hit_bad_trap = false;
    global_simulation_context = sim;  // 设置全局上下文

    global_sim_context = sim; // 设置全局指针

    // 关键字段初始化
    sim->last_valid_pc = PMEM_BASE;
    sim->pc_modifier = 0;
    sim->pc_modification_count = 0;
    #ifdef PC_UPDATE_LOCK
    sim->pc_lock = 0; // 初始解锁状态
    #endif
    return sim;
}

extern "C" void sim_free(SimContext* sim) {
    if (global_simulation_context == sim) { // 仅在全局上下文匹配时重置
        global_simulation_context = nullptr;
    }

    if (sim) {
        // 1. 关闭 NEMU 共享库
        if (sim->nemu_handle) {
            dlclose(sim->nemu_handle);
            sim->nemu_handle = nullptr;
        }
        
        // 2. 释放内存
        if (sim->memory) {
            delete[] sim->memory;
            sim->memory = nullptr;
        }
        
        // 3. 删除模拟器上下文
        delete sim;
        
        // 4. 重置全局指针
        global_sim_context = nullptr;
    }
}

extern "C" int sim_load_program(SimContext* sim, const char* program_path) {
    // 打开程序文件
    FILE* fp = fopen(program_path, "rb");
    if (!fp) {
        return -1;
    }
    
    // 获取文件大小
    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    
    // 检查文件是否过大
    if (file_size > PMEM_SIZE) {
        fclose(fp);
        fprintf(stderr, "Program too large: %ld bytes > %d bytes\n", file_size, PMEM_SIZE);
        return -1;
    }
    
    // 读取程序
    size_t read_size = fread(sim->memory + (PMEM_BASE - PMEM_BASE), 1, file_size, fp);
    fclose(fp);
    
    if (read_size != (size_t)file_size) {
        return -1;
    }
    
    // 设置初始PC
    sim->pc = PMEM_BASE;
    
    return 0;
}

#include <cstdio>
#include "sim_core.h"  // 包含必要的结构体定义

// 在 sim_core.cpp 中定义全局访问函数

extern "C" int pmem_read(int raddr) {
    SimContext* sim = get_sim_context();
    if (!sim) return -1;
    
    // 在访问前检查 PC 有效性
    if (sim->pc < PMEM_BASE || sim->pc > PMEM_END) {
        fprintf(stderr, 
            "[MEM] CRITICAL: Invalid PC 0x%08x during memory access\n",
            sim->pc
        );
        
        // 恢复最后有效 PC
        sim->pc = sim->last_valid_pc;
        sim->stop = true;
        return -1;
    }
    
    // 边界检查（保留原有逻辑）
    if (raddr < PMEM_BASE || raddr > PMEM_END) {
        fprintf(stderr, "[MEM] WARNING: Read out of bounds (0x%08x)\n", raddr);
        return -1;
    }
    
    uint32_t offset = raddr - PMEM_BASE;
    if (offset >= sim->mem_size) {
        fprintf(stderr, "[MEM] ERROR: Offset out of range (0x%08x)\n", raddr);
        return -1;
    }
    
    uint8_t* p = sim->memory + offset;
    return p[0] | (p[1] << 8) | (p[2] << 16) | (p[3] << 24);
}


extern "C" void pmem_write(int waddr, int wdata, char wmask) {
    SimContext* sim = get_sim_context();
    if (!sim) return; // 无上下文时不操作
    
    // 使用头文件中的宏定义
    if (waddr < PMEM_BASE || waddr > PMEM_END) {
        fprintf(stderr, "[MEM] WARNING: Write out of bounds: 0x%08x\n", waddr);
        return; // 越界不操作
    }
    
    // 计算偏移量（支持非对齐访问）
    uint32_t offset = waddr - PMEM_BASE;
    uint8_t* p = sim->memory + offset;
    
    // 应用写掩码（按字节写入）
    for (int i = 0; i < 4; i++) {
        if (wmask & (1 << i)) { // 检查第i字节是否需要写入
            p[i] = (wdata >> (i * 8)) & 0xFF;
        }
    }
}

extern "C" void get_regs(uint32_t regs[32]) {
    SimContext* sim = get_sim_context();
    if (sim) {
        memcpy(regs, sim->regs, sizeof(sim->regs)); // 复制寄存器值
    }
}

extern "C" void set_regs(uint32_t regs[32]) {
    SimContext* sim = get_sim_context();
    if (sim) {
        memcpy(sim->regs, regs, sizeof(sim->regs)); // 同步寄存器值
    }
}