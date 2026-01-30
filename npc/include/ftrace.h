#ifndef FTRACE_H
#define FTRACE_H

#include "common.h"


void ftrace_init(NPCContext* ctx);


// 记录一次函数调用/返回
// pc:当前指令的 PC
// target_pc:对于调用，目标为PC
// is_call:1为CALL,0为RET
void ftrace_log(uint64_t pc, uint64_t target_pc, int is_call);


//每次调用的 hook，目前主要NPC->C的npc_ftrace_log
void ftrace_step(NPCContext* ctx);

#endif
