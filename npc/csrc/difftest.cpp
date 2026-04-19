#include "difftest.h"
#include "dpi.h"
#include "common.h"

#if NPC_ENABLE_DIFFTEST

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

//必须与NEMU侧difftest-def.h中的方向定义保持一致
enum { DIFFTEST_TO_DUT = 0, DIFFTEST_TO_REF = 1 };

//difftest导出函数类型
typedef void (*ref_init_t)(int port);//初始化参考模型
typedef void (*ref_memcpy_t)(uint32_t addr, void *buf, size_t n, bool direction);//内存数据同步接口
typedef void (*ref_regcpy_t)(void *dut, bool direction);//寄存器状态同步接口
typedef void (*ref_exec_t)(uint64_t n);//执行n条指令

//保存动态库句柄和函数指针
static void *ref_handle = nullptr;//动态库句柄
static ref_init_t ref_init = nullptr;//初始化函数指针
static ref_memcpy_t ref_memcpy = nullptr;//内存同步函数指针
static ref_regcpy_t ref_regcpy = nullptr;//寄存器同步函数指针
static ref_exec_t ref_exec = nullptr;//执行函数指针

//CPU状态
typedef struct {
    uint32_t gpr[32];//32个通用寄存器
    uint32_t pc;//当前PC
} DifftestCPUState;

static inline int32_t sext32(uint32_t val, int bits) {
    uint32_t m = 1u << (bits - 1);
    return (int32_t)((val ^ m) - m);
}

static inline bool is_mmio_addr(uint32_t addr) {
    //AM常用MMIO空间: 0xa0000000~0xa1ffffff
    return addr >= 0xa0000000u && addr < 0xa2000000u;
}

static bool is_mmio_ls_inst(uint32_t inst, const uint32_t gpr[32]) {
    uint32_t opcode = inst & 0x7fu;
    if (opcode != 0x03u && opcode != 0x23u) {
        return false;
    }

    uint32_t rs1 = (inst >> 15) & 0x1fu;
    uint32_t base = gpr[rs1];

    int32_t imm = 0;
    if (opcode == 0x03u) { // load, I-type
        imm = sext32((inst >> 20) & 0xfffu, 12);
    } else { // store, S-type
        uint32_t simm = ((inst >> 25) << 5) | ((inst >> 7) & 0x1fu);
        imm = sext32(simm & 0xfffu, 12);
    }

    uint32_t addr = base + (uint32_t)imm;
    return is_mmio_addr(addr);
}


//从NPC上下文收集CPU状态到DifftestCPUState结构体
static void npc_collect_cpu_state(DifftestCPUState *st, NPCContext *ctx) {
    uint32_t gpr_all[REG_NUM] = {0};
    npc_get_regs(gpr_all);//从NPC读取RV32E寄存器


    if(REG_NUM == 16) {//RV32E只有16个寄存器,其余寄存器按0处理
        for (int i = 0; i < 16; i++) {
            st->gpr[i] = gpr_all[i];
        }
        for (int i = 16; i < 32; i++) {
            st->gpr[i] = 0;
        }
    }
    else {
        for (int i = 0; i < REG_NUM; i++) {
            st->gpr[i] = gpr_all[i];
        }
    }
    st->pc = ctx->pc;//记录NPC当前指令执行后的PC
}


//尝试打开NEMU共享库，返回
static void* try_open_so(const char **used_path) {
    //优先匹配RV32E参考模型,再回退到RV32I参考模型
    static const char* REF_CANDIDATES[] = {
        "/home/pz40/ysyx-workbench/nemu/build/riscv32e-nemu-interpreter-so",
        "/home/pz40/ysyx-workbench/nemu/build/riscv32-nemu-interpreter-so",
    };

    for (size_t i = 0; i < sizeof(REF_CANDIDATES) / sizeof(REF_CANDIDATES[0]); i++) {
        void *handle = dlopen(REF_CANDIDATES[i], RTLD_LAZY);//加载动态库
        if (handle) {
            *used_path = REF_CANDIDATES[i];
            return handle;
        }
    }

    *used_path = REF_CANDIDATES[0];//记录首选路径,便于报错定位
    return nullptr;
}


//初始化difftest环境并把DUT初始状态同步到REF
void difftest_init(NPCContext* ctx) {
    if (!ctx->debug.difftest_enabled) {
        return;
    }

    const char *used_path = nullptr;//记录最终命中的so路径,用于日志输出
    ref_handle = try_open_so(&used_path);//尝试加载动态库

    if (!ref_handle) {//加载失败则打印详细错误信息并退出
        char cwd_buf[512];
        const char *cwd = getcwd(cwd_buf, sizeof(cwd_buf));
        fprintf(stderr,
            "DiffTest: failed to load NEMU shared object.\n""Current working directory: %s\n"
            "Tried paths:\n"
            "  /home/pz40/ysyx-workbench/nemu/build/riscv32e-nemu-interpreter-so\n"
            "  /home/pz40/ysyx-workbench/nemu/build/riscv32-nemu-interpreter-so\n",
            cwd ? cwd : "(unknown)");

        exit(1);
    }

    ref_init = (ref_init_t) dlsym(ref_handle, "difftest_init");//初始化模型
    ref_memcpy = (ref_memcpy_t) dlsym(ref_handle, "difftest_memcpy");//内存数据同步接口
    ref_regcpy = (ref_regcpy_t) dlsym(ref_handle, "difftest_regcpy");//寄存器状态同步接口
    ref_exec = (ref_exec_t) dlsym(ref_handle, "difftest_exec");//执行n条指令
    //检查关键符号是否齐全,避免因空函数指针而崩溃

    if (!ref_init || !ref_memcpy || !ref_regcpy || !ref_exec) {//符号缺失则打印错误并退出
        printf("DiffTest: missing symbol(s) in NEMU shared object\n");
        dlclose(ref_handle);
        ref_handle = nullptr;
        exit(1);
    }

    printf("DiffTest: using NEMU so: %s\n", used_path ? used_path : "(unknown)");//输出最终使用的so路径

    //初始化NEMUdifftest环境
    ref_init(0);


    //把NPC内存镜像拷贝到NEMU，如果不先把NPC的内存拷给NEMU，两边取到的指令/数据会不同，第一批指令就可能跑偏
    const uint32_t img_sync_size = MEM_SIZE;
    //同步范围直接跟随主存配置,避免镜像超范围时出现假失配
    for (uint32_t off = 0; off < img_sync_size; off += 4) {//按4字节对齐同步内存,与difftest_memcpy的接口一致

        //npc_pmem_read的参数是相对MEM_BASE的物理偏移
        uint32_t word = npc_pmem_read((int)off);
        ref_memcpy(MEM_BASE + off, &word, 4, DIFFTEST_TO_REF);//把NPC内存数据同步到REF
    }

    //初始化CPU寄存器状态
    DifftestCPUState cpu_init{};
    for (int i = 0; i < 32; i++) {
        cpu_init.gpr[i] = 0;//NPC的未实现寄存器初始化为0
    }
    cpu_init.pc = ctx->entry;//入口地址

    ref_regcpy(&cpu_init, DIFFTEST_TO_REF);
    //先把DUT初始寄存器态推给REF,确保首条指令比较时两边起点一致

    printf("DiffTest initialized successfully\n");
}


//=执行一步difftest并比较状态
void difftest_step(NPCContext* ctx) {
    if (!ctx->debug.difftest_enabled) {//
        return;
    }
    if (!ref_handle) {
        return;
    }

    //捕捉当前指令引起的状态变化，先保存执行前的状态，再执行REF，最后保存执行后的状态进行比较
    DifftestCPUState ref_before{};//执行前的REF状态，记录触发状态变化的指令PC
    ref_regcpy(&ref_before, DIFFTEST_TO_DUT);//先把REF当前寄存器态同步到DUT,便于后面比较
    //先抓ref_before.pc,用于把后续不一致精准定位到“哪一条指令”触发

    //对MMIO的load/store不在REF执行
    if (ref_before.pc >= MEM_BASE && ref_before.pc < MEM_BASE + MEM_SIZE) {
        uint32_t inst = (uint32_t)npc_pmem_read(ref_before.pc - MEM_BASE);
        if (is_mmio_ls_inst(inst, ref_before.gpr)) {
            DifftestCPUState dut_after{};
            npc_collect_cpu_state(&dut_after, ctx);
            ref_regcpy(&dut_after, DIFFTEST_TO_REF);
            return;
        }
    }


    ref_exec(1);


    DifftestCPUState ref_after{};//执行后的REF状态
    ref_regcpy(&ref_after, DIFFTEST_TO_DUT);

    //获取NPC当前状态
    DifftestCPUState dut_after{};//执行后的DUT(NPC)状态
    npc_collect_cpu_state(&dut_after, ctx);//比较的是“同一条指令执行后”的DUT/REF状态,不是执行前状态

    //指令PC
    uint32_t instr_pc = ref_before.pc;//触发本次状态变化的指令PC

    //比较PC
    if (ref_after.pc != dut_after.pc) {//PC不一致
        printf("[DiffTest] PC mismatch at instruction PC = 0x%08x\n", instr_pc);
        printf("        Next PC after this instruction: DUT=0x%08x, REF=0x%08x\n",
               dut_after.pc, ref_after.pc);
        printf("Hint: This usually indicates a bug in control-flow logic, e.g.\n");
        printf("      - wrong jump/branch target calculation\n");
        printf("      - incorrect pc+4 update\n");
        printf("      - different behavior on ebreak/exception/return\n");
        ctx->stop = true;//主循环停机
        ctx->stop_reason = (char*)"DiffTest PC mismatch";
        //pc不一致则立即停机
        return;
    }


    //比较寄存器
    if(REG_NUM == 16) {
        for (int i = 0; i < 16; i++) {
            if (ref_after.gpr[i] != dut_after.gpr[i]) {
                printf("[DiffTest] Register mismatch at instruction PC = 0x%08x\n",instr_pc);
                printf("           x%-2d: DUT=0x%08x, REF=0x%08x\n",i, dut_after.gpr[i], ref_after.gpr[i]);
                printf("Hint: The bug is very likely in the implementation of\n");
                printf("      the instruction at 0x%08x, or in the write-back\n", instr_pc);
                printf("      path for register x%d (ALU result, load data, etc.).\n", i);
                ctx->stop = true;
                ctx->stop_reason = (char*)"DiffTest reg mismatch";
                return;
            }
        }
    }
    else {
        for (int i = 0; i < 32; i++) {
            if (ref_after.gpr[i] != dut_after.gpr[i]) {
                printf("[DiffTest] Register mismatch at instruction PC = 0x%08x\n",instr_pc);
                printf("           x%-2d: DUT=0x%08x, REF=0x%08x\n",i, dut_after.gpr[i], ref_after.gpr[i]);
                printf("Hint: The bug is very likely in the implementation of\n");
                printf("      the instruction at 0x%08x, or in the write-back\n", instr_pc);
                printf("      path for register x%d (ALU result, load data, etc.).\n", i);
                ctx->stop = true;
                ctx->stop_reason = (char*)"DiffTest reg mismatch";
                return;
            }
        }
    }
}

#else


void difftest_init(NPCContext* ctx) { (void)ctx; }
void difftest_step(NPCContext* ctx) { (void)ctx; }

#endif