#include "sdb.h"
#include "sim_core.h"
#include "trace.h"
#include "difftest.h"
#include <readline/readline.h>
#include <readline/history.h>
#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <vector>
#include <string>
#include <map>

static SimContext* current_sim = nullptr; // 当前仿真上下文
static int watchpoint_count = 0;          // 监视点计数器
static std::map<int, Watchpoint> watchpoints; // 监视点列表

// 使用全局访问函数
extern "C" SimContext* get_sim_context();
// 寄存器名称映射（RV32E）
static const char* reg_names[32] = {
    "zero", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
    "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
    "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
    "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};

// 寄存器写辅助函数
static void write_register(SimContext* sim, int reg_idx, uint32_t value) {
    if (reg_idx == 0 || reg_idx >= 16) return;
    sim->reg_write_enabled[reg_idx] = true;
    sim->regs[reg_idx] = value;
}

// 获取寄存器值（通过SimContext）
static uint32_t get_reg_value(const char* name) {
    uint32_t regs[32];
    get_regs(regs); // 从SimContext获取寄存器值
    
    // 检查寄存器名称（如"x1"）
    if (name[0] == 'x' && strlen(name) == 2 && isdigit(name[1])) {
        int idx = name[1] - '0';
        if (idx >= 0 && idx < 32) {
            return regs[idx];
        }
    }
    
    // 检查寄存器别名（如"ra"对应x1）
    for (int i = 0; i < 32; i++) {
        if (strcmp(name, reg_names[i]) == 0) {
            return regs[i];
        }
    }
    
    return 0; // 未找到
}

// 表达式求值（支持寄存器、内存、常量）
uint32_t expr_eval(const char* expr) {
    if (!expr) return 0;
    
    // 处理十六进制常量（如0x1234）
    if (expr[0] == '0' && expr[1] == 'x') {
        return strtoul(expr, nullptr, 16);
    }
    
    // 处理十进制常量（如1234）
    if (isdigit(expr[0])) {
        return strtoul(expr, nullptr, 10);
    }
    
    // 处理内存地址（如&0x1000）
    if (expr[0] == '&') {
        uint32_t addr = expr_eval(expr + 1); // 递归求值地址
        return pmem_read(addr); // 读取内存值
    }
    
    // 处理寄存器（如x1或ra）
    return get_reg_value(expr);
}

// 添加监视点
int add_watchpoint(const char* expr) {
    Watchpoint wp;
    wp.id = ++watchpoint_count;
    wp.expr = expr;
    wp.last_value = expr_eval(expr); // 初始值
    watchpoints[wp.id] = wp;
    return wp.id;
}

// 删除监视点
bool delete_watchpoint(int id) {
    auto it = watchpoints.find(id);
    if (it != watchpoints.end()) {
        watchpoints.erase(it);
        return true;
    }
    return false;
}

// 列出监视点
void list_watchpoints() {
    if (watchpoints.empty()) {
        printf("No watchpoints\n");
        return;
    }
    
    printf("ID\tExpression\tLast Value\n");
    for (const auto& pair : watchpoints) {
        printf("%d\t%s\t0x%08x\n", pair.first, pair.second.expr.c_str(), pair.second.last_value);
    }
}

// 检查监视点（触发条件：表达式值变化）
void check_watchpoints() {
    for (auto& pair : watchpoints) {
        int id = pair.first;
        Watchpoint& wp = pair.second;
        
        uint32_t current_value = expr_eval(wp.expr.c_str());
        if (current_value != wp.last_value) {
            printf("Watchpoint %d triggered: %s changed from 0x%08x to 0x%08x\n",
                   id, wp.expr.c_str(), wp.last_value, current_value);
            wp.last_value = current_value;
            
            // 触发调试暂停
            if (current_sim) {
                current_sim->stop = true;
            }
        }
    }
}

// 打印寄存器（简化版）
static void print_regs() {
    uint32_t regs[32];
    get_regs(regs);
    
    printf("Registers:\n");
    for (int i = 0; i < 32; i++) {
        printf("%-4s (x%2d): 0x%08x", reg_names[i], i, regs[i]);
        if (i % 4 == 3) printf("\n"); // 每行4个
    }
    printf("\n");
}

// 扫描内存（简化版）
static void scan_memory(uint32_t addr, int count) {
    printf("Memory at 0x%08x:\n", addr);
    for (int i = 0; i < count; i++) {
        uint32_t value = pmem_read(addr + i * 4);
        printf("0x%08x: 0x%08x\n", addr + i * 4, value);
    }
}


// 检查是否为跳转指令
static bool is_jump_instruction(uint32_t inst) {
    uint8_t opcode = inst & 0x7F;
    return (opcode == 0x6F) ||  // JAL
           (opcode == 0x67) ||  // JALR
           (opcode == 0x63);    // 分支指令 (BEQ, BNE, etc.)
}

// 计算跳转目标地址
static uint32_t calculate_jump_target(uint32_t pc, uint32_t inst) {
    uint8_t opcode = inst & 0x7F;
    
    if (opcode == 0x6F) { // JAL
        int32_t imm = ((inst >> 31) ? 0xFFFFF000 : 0) |
                      ((inst >> 12) & 0xFF) << 12 |
                      ((inst >> 20) & 0x1) << 11 |
                      ((inst >> 21) & 0x3FF) << 1;
        return pc + imm;
    }
    else if (opcode == 0x67) { // JALR
        uint8_t rs1 = (inst >> 15) & 0x1F;
        int32_t imm = (inst >> 20) & 0xFFF;
        if (imm & 0x800) imm |= 0xFFFFF000; // 符号扩展
        return (current_sim->regs[rs1] + imm) & ~1; // 清除最低位
    }
    else if (opcode == 0x63) { // 分支指令
        int32_t imm = ((inst >> 31) ? 0xFFFFF000 : 0) |
                      ((inst >> 7) & 0x1) << 11 |
                      ((inst >> 25) & 0x3F) << 5 |
                      ((inst >> 8) & 0xF) << 1;
        return pc + imm;
    }
    
    return pc + 4; // 默认返回下一条指令
}


// 状态完整性检查（关键修复）
void check_state_integrity(SimContext* sim) {
    static uint32_t prev_pc = PMEM_BASE;
    static uint32_t prev_regs[16] = {0};
    
    // PC 突变检查
    if (sim->pc != prev_pc + 4 && prev_pc != 0) {
        if (sim->pc != PMEM_BASE) {
            fprintf(stderr, "[ALERT] Unexpected PC jump: 0x%08x → 0x%08x\n",
                    prev_pc, sim->pc);
        }
    }
    prev_pc = sim->pc;
    
    // 寄存器突变检查（使用正确的成员名）
    for (int i = 1; i < 16; i++) {
        if (sim->regs[i] != prev_regs[i] && !sim->reg_write_enabled[i]) {
            fprintf(stderr, "[ALERT] Unexpected change: x%02d: 0x%08x → 0x%08x\n",
                    i, prev_regs[i], sim->regs[i]);
        }
        prev_regs[i] = sim->regs[i];
    }
}

static uint64_t instruction_counter = 0;

// 执行指令（单周期）
static void execute_instruction() {
    instruction_counter++;

    SimContext* sim = get_sim_context();
    
    if (!sim) {
        fprintf(stderr, "[CPU] ERROR: No simulation context\n");
        return;
    }
    
    // 重置寄存器写标志
    memset(sim->reg_write_enabled, 0, sizeof(sim->reg_write_enabled));
    
    // 记录执行前状态
    fprintf(stderr, "[BEFORE] Instr#%lu: PC=0x%08x, Reg[8]=0x%08x\n", 
            instruction_counter, sim->pc, sim->regs[8]);
    
    // ===========================================
    // 0. 解锁 PC
    // ===========================================
    #ifdef PC_UPDATE_LOCK
    sim->pc_lock = 0;  // 解锁 PC
    #endif
    
    // ===========================================
    // 1. 保存当前状态
    // ===========================================
    const uint32_t current_pc = sim->pc;
    uint32_t next_pc = current_pc + 4;  // 默认下一条指令地址
    
    // ===========================================
    // 2. PC 有效性检查
    // ===========================================
    if (current_pc < PMEM_BASE || current_pc > PMEM_END) {
        fprintf(stderr, "[CPU] CRITICAL: Invalid PC 0x%08x\n", current_pc);
        safe_set_pc(sim, PMEM_BASE);
        return;
    }

    // ===========================================
    // 3. 指令读取
    // ===========================================
    int inst_val = pmem_read(current_pc);
    
    if (inst_val == -1) {
        fprintf(stderr, "[CPU] ERROR: Invalid memory access at PC=0x%08x\n", current_pc);
        sim->stop = true;
        sim->hit_bad_trap = true;
        return;
    }
    
    uint32_t inst = static_cast<uint32_t>(inst_val);
    printf("[DEBUG] EXEC: PC=0x%08x, INST=0x%08x\n", current_pc, inst);
    log_itrace(current_pc, inst);
    
    // ===========================================
    // 4. 指令格式验证
    // ===========================================
    if ((inst & 0x3) != 0x3) {
        fprintf(stderr, 
            "[CPU] ERROR: Invalid instruction format at PC=0x%08x (INST=0x%08x)\n",
            current_pc, inst
        );
        safe_set_pc(sim, current_pc + 4);
        return;
    }
    
    // ===========================================
    // 5. 指令解码
    // ===========================================
    uint8_t opcode = inst & 0x7F;
    uint8_t funct3 = (inst >> 12) & 0x7;
    uint8_t funct7 = (inst >> 25) & 0x7F;
    
    uint8_t rd = (inst >> 7) & 0x1F;
    uint8_t rs1 = (inst >> 15) & 0x1F;
    uint8_t rs2 = (inst >> 20) & 0x1F;
    
    // 提取立即数
    int32_t imm_i = (inst >> 20);
    int32_t imm_s = ((inst >> 7) & 0x1F) | ((inst >> 25) << 5);
    int32_t imm_b = ((inst >> 7) & 0x1E) | ((inst >> 20) & 0x7E0) | 
                   ((inst << 4) & 0x800) | ((inst >> 19) & 0x1000);
    int32_t imm_u = inst & 0xFFFFF000;
    int32_t imm_j = ((inst >> 21) & 0x3FF) | ((inst >> 20) & 0x800) |
                   ((inst >> 9) & 0x7FE) | ((inst >> 31) ? 0xFFF00000 : 0);
    
    // 符号扩展
    if (imm_i & 0x800) imm_i |= 0xFFFFF000;
    if (imm_s & 0x800) imm_s |= 0xFFFFF000;
    if (imm_b & 0x1000) imm_b |= 0xFFFFE000;
    
    // 在指令解码后添加
    if (opcode == 0x6F || opcode == 0x67 || opcode == 0x63 || opcode == 0x03 || opcode == 0x23) {
    // 对于分支、跳转、加载/存储指令，额外检查 next_pc 合法性
    if (next_pc < PMEM_BASE || next_pc > PMEM_END) {
        fprintf(stderr, "[WARNING] Suspicious next_pc: 0x%08x (from opcode 0x%02x)\n", 
                next_pc, opcode);
    }
}



    // 保存执行前状态
    const uint32_t pre_pc = sim->pc;
    const uint32_t pre_last_valid = sim->last_valid_pc;
    // ===========================================
    // 6. 指令执行（支持所有 RV32E 指令）
    // ===========================================
    switch (opcode) {
        // LUI (Load Upper Immediate)
        case 0x37:
            if (rd != 0 && rd < 16) write_register(sim, rd, imm_u);
            break;
            
        // AUIPC (Add Upper Immediate to PC)
        case 0x17:
            if (rd != 0 && rd < 16) write_register(sim, rd, current_pc + imm_u);
            break;
            
        // JAL (Jump and Link)
        case 0x6F:
            if (rd != 0 && rd < 16) write_register(sim, rd, current_pc + 4);
            next_pc = current_pc + imm_j;
            break;
            
        // JALR (Jump and Link Register)
        case 0x67:
            if (rd != 0 && rd < 16) write_register(sim, rd, current_pc + 4);
            if (rs1 < 16) {
                next_pc = (sim->regs[rs1] + imm_i) & ~1;
            } else {
                fprintf(stderr, "[CPU] ERROR: Invalid rs1 register %d at PC=0x%08x\n", rs1, current_pc);
                next_pc = current_pc + 4;
            }
            break;
            
        // 分支指令 (BEQ, BNE, BLT, BGE, BLTU, BGEU)
        case 0x63: {
            bool branch_taken = false;
            uint32_t rs1_val = (rs1 < 16) ? sim->regs[rs1] : 0;
            uint32_t rs2_val = (rs2 < 16) ? sim->regs[rs2] : 0;
            
            switch (funct3) {
                case 0x0: branch_taken = (rs1_val == rs2_val); break; // BEQ
                case 0x1: branch_taken = (rs1_val != rs2_val); break; // BNE
                case 0x4: branch_taken = ((int32_t)rs1_val < (int32_t)rs2_val); break; // BLT
                case 0x5: branch_taken = ((int32_t)rs1_val >= (int32_t)rs2_val); break; // BGE
                case 0x6: branch_taken = (rs1_val < rs2_val); break; // BLTU
                case 0x7: branch_taken = (rs1_val >= rs2_val); break; // BGEU
            }
            
            if (branch_taken) {
                next_pc = current_pc + imm_b;
            }
            break;
        }
        
        // 加载指令 (LB, LH, LW, LBU, LHU)
        case 0x03: {
            uint32_t addr = (rs1 < 16) ? sim->regs[rs1] + imm_i : 0;
            uint32_t value = pmem_read(addr);
            
            switch (funct3) {
                case 0x0: value = (int8_t)(value & 0xFF); break; // LB
                case 0x1: value = (int16_t)(value & 0xFFFF); break; // LH
                case 0x2: break; // LW (无需处理)
                case 0x4: value = value & 0xFF; break; // LBU
                case 0x5: value = value & 0xFFFF; break; // LHU
            }
            
            if (rd != 0 && rd < 16) write_register(sim, rd, value);
            break;
        }
        
        // 存储指令 (SB, SH, SW)
        case 0x23: {
            uint32_t addr = (rs1 < 16) ? sim->regs[rs1] + imm_s : 0;
            uint32_t value = (rs2 < 16) ? sim->regs[rs2] : 0;
            char wmask = 0;
            
            switch (funct3) {
                case 0x0: wmask = 0x1; value &= 0xFF; break; // SB
                case 0x1: wmask = 0x3; value &= 0xFFFF; break; // SH
                case 0x2: wmask = 0xF; break; // SW
            }
            
            pmem_write(addr, value, wmask);
            break;
        }
        
        // 立即数运算指令 (ADDI, SLTI, SLTIU, ANDI, ORI, XORI, SLLI, SRLI, SRAI)
        case 0x13: {
            uint32_t rs1_val = (rs1 < 16) ? sim->regs[rs1] : 0;
            uint32_t result = 0;
            
            switch (funct3) {
                case 0x0: result = rs1_val + imm_i; break; // ADDI
                case 0x1: result = rs1_val << (imm_i & 0x1F); break; // SLLI
                case 0x2: result = ((int32_t)rs1_val < imm_i) ? 1 : 0; break; // SLTI
                case 0x3: result = (rs1_val < (uint32_t)imm_i) ? 1 : 0; break; // SLTIU
                case 0x4: result = rs1_val ^ imm_i; break; // XORI
                case 0x5: 
                    if (funct7 == 0x00) result = rs1_val >> (imm_i & 0x1F); // SRLI
                    else if (funct7 == 0x20) result = (int32_t)rs1_val >> (imm_i & 0x1F); // SRAI
                    break;
                case 0x6: result = rs1_val | imm_i; break; // ORI
                case 0x7: result = rs1_val & imm_i; break; // ANDI
            }
            
            if (rd != 0 && rd < 16) write_register(sim, rd, result);
            break;
        }
        
        // 寄存器运算指令 (ADD, SUB, SLL, SLT, SLTU, XOR, SRL, SRA, OR, AND)
        case 0x33: {
            uint32_t rs1_val = (rs1 < 16) ? sim->regs[rs1] : 0;
            uint32_t rs2_val = (rs2 < 16) ? sim->regs[rs2] : 0;
            uint32_t result = 0;
            
            switch (funct3) {
                case 0x0: 
                    if (funct7 == 0x00) result = rs1_val + rs2_val; // ADD
                    else if (funct7 == 0x20) result = rs1_val - rs2_val; // SUB
                    break;
                case 0x1: result = rs1_val << (rs2_val & 0x1F); break; // SLL
                case 0x2: result = ((int32_t)rs1_val < (int32_t)rs2_val) ? 1 : 0; break; // SLT
                case 0x3: result = (rs1_val < rs2_val) ? 1 : 0; break; // SLTU
                case 0x4: result = rs1_val ^ rs2_val; break; // XOR
                case 0x5: 
                    if (funct7 == 0x00) result = rs1_val >> (rs2_val & 0x1F); // SRL
                    else if (funct7 == 0x20) result = (int32_t)rs1_val >> (rs2_val & 0x1F); // SRA
                    break;
                case 0x6: result = rs1_val | rs2_val; break; // OR
                case 0x7: result = rs1_val & rs2_val; break; // AND
            }
            
            if (rd != 0 && rd < 16) write_register(sim, rd, result);
            break;
        }
        
        // FENCE 指令
        case 0x0F: // FENCE
            // 空操作（无实际效果）
            break;
            
        // 系统指令 (ECALL, EBREAK)
        case 0x73: {
            if (funct3 == 0x0) {
                if (imm_i == 0x0) { // ECALL
                    sim->stop = true;
                    printf("ECALL encountered at pc=0x%08x\n", current_pc);
                } else if (imm_i == 0x1) { // EBREAK
                    sim->hit_good_trap = true;
                    sim->stop = true;
                    printf("EBREAK encountered at pc=0x%08x\n", current_pc);
                }
            }
            break;
        }
        
        // 其他指令（未知）
        default:
            fprintf(stderr, 
                "[CPU] ERROR: Unknown instruction at PC=0x%08x (OPCODE=0x%02X)\n",
                current_pc, opcode
            );
            break;
    }



    // 执行后检查 PC 是否被篡改
    if (sim->pc != next_pc) {
        fprintf(stderr, 
            "\n[CRITICAL] PC CORRUPTION DETECTED!\n"
            "  Before exec: 0x%08x\n"
            "  Expected:    0x%08x\n"
            "  Actual:      0x%08x\n"
            "  Last valid:  0x%08x\n",
            pre_pc, next_pc, sim->pc, pre_last_valid
        );
        
        // 恢复最后有效 PC
        sim->pc = pre_last_valid;
        sim->stop = true;
        return;
    }
    // ===========================================
    // 7. 下一条指令地址验证
    // ===========================================
    if (next_pc < PMEM_BASE || next_pc > PMEM_END) {
        fprintf(stderr, 
            "[CPU] ERROR: Invalid next PC 0x%08x at PC=0x%08x (INST=0x%08x)\n",
            next_pc, current_pc, inst
        );
        next_pc = current_pc + 4;
    }
    
    // ===========================================
    // 8. 安全更新 PC
    // ===========================================
    printf("[EXU_PC] Pre-Branch: PC=0x%08x, Next=0x%08x\n", current_pc, next_pc);
    safe_set_pc(sim, next_pc);
    
    // ===========================================
    // 9. 更新指令计数器
    // ===========================================
    sim->total_instructions++;
    
    // ===========================================
    // 10. 重新锁定 PC
    // ===========================================
    #ifdef PC_UPDATE_LOCK
    sim->pc_lock = 1;  // 执行完成后锁定 PC
    #endif
    
    // ===========================================
    // 11. 特殊处理
    // ===========================================
    // DiffTest
    if (sim->diff_test_enabled) {
        if (!do_diff_test(sim)) {
            printf("DiffTest failed at pc=0x%08x, inst=0x%08x\n", current_pc, inst);
            sim->stop = true;
            sim->hit_bad_trap = true;
        }
    }
    
    // 检查监视点
    check_watchpoints();

    // 状态完整性检查
    check_state_integrity(sim);
    
    // 记录执行后状态
    fprintf(stderr, "[AFTER]  Instr#%lu: PC=0x%08x, Next=0x%08x, Reg[8]=0x%08x\n\n", 
            instruction_counter, sim->pc, next_pc, sim->regs[8]);
}

// SDB命令解析（简化版）
static int cmd_c(char* args) {
    if (!current_sim) {
        fprintf(stderr, "[SDB] ERROR: No simulation context\n");
        return 0;
    }
    
    current_sim->stop = false;
    printf("Continuing execution...\n");
    
    // 添加最大指令计数限制
    const int max_inst_per_cmd = 1000000;
    int inst_count = 0;
    
    while (!current_sim->stop && 
           current_sim->total_instructions < current_sim->max_cycles &&
           inst_count < max_inst_per_cmd) {
        execute_instruction();
        inst_count++;
        
        // 添加检查，避免长时间阻塞
        if (inst_count % 10000 == 0) {
            printf("Executed %d instructions...\n", inst_count);
        }
    }
    
    if (inst_count >= max_inst_per_cmd) {
        printf("Hit max instructions per command (%d), stopping execution\n", max_inst_per_cmd);
    }
    
    return 0;
}

static int cmd_si(char* args) {
    if (!current_sim) {
        fprintf(stderr, "[SDB] ERROR: No simulation context\n");
        return 0;
    }
    
    int n = 1;
    if (args) n = atoi(args);
    if (n <= 0) n = 1;
    
    // 限制单次执行的指令数量
    if (n > 1000) {
        printf("Warning: Limiting si to 1000 instructions\n");
        n = 1000;
    }
    
    for (int i = 0; i < n; i++) {
        if (current_sim->stop || 
            current_sim->total_instructions >= current_sim->max_cycles) {
            break;
        }
        execute_instruction();
    }
    
    return 0;
}

static int cmd_info(char* args) {
    if (!args || strcmp(args, "r") == 0) {
        print_regs(); // 打印寄存器
    } else if (strcmp(args, "w") == 0) {
        list_watchpoints(); // 列出监视点
    } else {
        printf("Unknown info command: %s\n", args);
    }
    return 0;
}

static int cmd_x(char* args) {
    char* addr_str = strtok(args, " ");
    char* count_str = strtok(nullptr, " ");
    if (!addr_str || !count_str) {
        printf("Usage: x ADDR COUNT\n");
        return 0;
    }
    uint32_t addr = expr_eval(addr_str);
    int count = atoi(count_str);
    scan_memory(addr, count);
    return 0;
}

static int cmd_w(char* args) {
    if (!args) {
        printf("Usage: w EXPR\n");
        return 0;
    }
    int id = add_watchpoint(args);
    printf("Added watchpoint %d: %s\n", id, args);
    return 0;
}

static int cmd_d(char* args) {
    if (!args) {
        printf("Usage: d ID\n");
        return 0;
    }
    int id = atoi(args);
    if (delete_watchpoint(id)) {
        printf("Deleted watchpoint %d\n", id);
    } else {
        printf("No such watchpoint: %d\n", id);
    }
    return 0;
}

static int cmd_p(char* args) {
    if (!args) {
        printf("Usage: p EXPR\n");
        return 0;
    }
    uint32_t value = expr_eval(args);
    printf("%s = 0x%08x\n", args, value);
    return 0;
}

#include <dlfcn.h>  // 用于动态加载 NEMU 共享库

static int cmd_q(char* args) {
    return 0;
}

// SDB主循环
void sdb_mainloop(SimContext* sim, int max_cycles) {
    // 使用局部变量保存上下文指针
    SimContext* local_sim = sim;
    local_sim->max_cycles = max_cycles;
    
    char* line;
    printf("NPC SDB (type 'help' for commands)\n");

    // 使用局部退出标志
    bool should_exit = false;
    
    while (!local_sim->stop && 
           local_sim->total_instructions < local_sim->max_cycles &&
           !should_exit) {
        line = readline("(npc) ");
        if (!line) break; // EOF
        
        char* cmd = strtok(line, " ");
        if (!cmd) {
            free(line);
            continue;
        }
        
        char* args = strtok(nullptr, "");
        
        // 命令分发（不使用任何 sim 成员）
        if (strcmp(cmd, "c") == 0) {
            cmd_c(args);
        } else if (strcmp(cmd, "si") == 0) {
            cmd_si(args);
        } else if (strcmp(cmd, "info") == 0) {
            cmd_info(args);
        } else if (strcmp(cmd, "x") == 0) {
            cmd_x(args);
        } else if (strcmp(cmd, "w") == 0) {
            cmd_w(args);
        } else if (strcmp(cmd, "d") == 0) {
            cmd_d(args);
        } else if (strcmp(cmd, "p") == 0) {
            cmd_p(args);
        } else if (strcmp(cmd, "q") == 0) {
            // 设置局部退出标志
            printf("Exiting SDB...\n");
            should_exit = true;
        } else {
            printf("Unknown command: %s\n", cmd);
        }

        free(line);
        
        if (should_exit) {
            break; // 退出主循环
        }
    }

    // 安全退出（不访问任何 sim 成员）
    printf("SDB exited safely\n");
    
    // 重要：不释放任何资源，由调用者负责
}

// 初始化SDB
void init_sdb(SimContext* sim) {
    current_sim = sim;
    using_history(); // 启用历史记录
}