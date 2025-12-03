#ifndef FTRACE_H
#define FTRACE_H

#include "common.h"

// 初始化函数追踪（解析符号表等）
void ftrace_init(NPCContext* ctx);

// 记录一次函数调用/返回
// pc        : 当前指令的 PC
// target_pc : 对于调用，目标 PC；对于返回，可以忽略
// is_call   : 1 = CALL, 0 = RET
void ftrace_log(uint64_t pc, uint64_t target_pc, int is_call);

// 每拍调用的 hook，目前主要靠 NPC -> C 的 npc_ftrace_log
void ftrace_step(NPCContext* ctx);

#endif
