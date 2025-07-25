#include "inst_decode.h"  // 自定义头文件
#include <cstdio>         // 标准输入输出
#include <cstring>        // 字符串操作

// -------------------------------
// 指令解码核心逻辑（保留原始实现）
// -------------------------------
void decode_inst(InstInfo* info) {
    uint32_t inst = info->inst;
    
    // 提取操作码（低7位）
    uint32_t opcode = inst & 0x7F;
    
    // 初始化默认值
    info->type = TYPE_UNKNOWN;
    strncpy(info->name, "unknown", sizeof(info->name)-1);
    info->name[sizeof(info->name)-1] = '\0';
    snprintf(info->op_str, sizeof(info->op_str), "0x%08x", inst);
    
    // RV32E指令类型解码（覆盖常见指令）
    switch (opcode) {
        case 0x03: // I-type: LOAD
            info->type = TYPE_I;
            strncpy(info->name, "lw", sizeof(info->name)-1);
            break;
        case 0x13: // I-type: OP-IMM（ADDI/XORI/ORI）
            info->type = TYPE_I;
            if ((inst & 0x7000) == 0x0000) {  // ADDI
                strncpy(info->name, "addi", sizeof(info->name)-1);
            } else if ((inst & 0x7000) == 0x4000) {  // XORI
                strncpy(info->name, "xori", sizeof(info->name)-1);
            } else if ((inst & 0x7000) == 0x6000) {  // ORI
                strncpy(info->name, "ori", sizeof(info->name)-1);
            }
            break;
        case 0x23: // S-type: STORE
            info->type = TYPE_S;
            strncpy(info->name, "sw", sizeof(info->name)-1);
            break;
        case 0x33: // R-type: OP（ADD/SUB）
            info->type = TYPE_R;
            if ((inst & 0x7000) == 0x0000) {  // ADD
                strncpy(info->name, "add", sizeof(info->name)-1);
            } else if ((inst & 0x7000) == 0x4000) {  // SUB
                strncpy(info->name, "sub", sizeof(info->name)-1);
            }
            break;
        case 0x63: // B-type: BRANCH（BEQ/BNE）
            info->type = TYPE_B;
            if ((inst & 0x7000) == 0x0000) {  // BEQ
                strncpy(info->name, "beq", sizeof(info->name)-1);
            } else if ((inst & 0x7000) == 0x1000) {  // BNE
                strncpy(info->name, "bne", sizeof(info->name)-1);
            }
            break;
        default:
            // 未知指令类型（保留原始操作码）
            snprintf(info->op_str, sizeof(info->op_str), "0x%08x (unknown)", inst);
            break;
    }
}