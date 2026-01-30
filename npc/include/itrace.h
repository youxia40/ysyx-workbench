#ifndef ITRACE_H
#define ITRACE_H

#include "common.h"

void itrace_init(NPCContext* ctx);


//单步指令追踪
void itrace_step(NPCContext* ctx);


//反汇编一条
void itrace_disassemble(uint64_t pc, uint32_t inst);

#endif
