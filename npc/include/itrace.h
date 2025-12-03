#ifndef ITRACE_H
#define ITRACE_H

#include "common.h"

// 初始化指令追踪系统
void itrace_init(NPCContext* ctx);

// 单步指令追踪（每条指令执行后调用）
void itrace_step(NPCContext* ctx);

// 手动反汇编一条指令
void itrace_disassemble(uint64_t pc, uint32_t inst);

#endif
