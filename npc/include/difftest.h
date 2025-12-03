#ifndef DIFFTEST_H
#define DIFFTEST_H

#include "common.h"

// 初始化差分测试（在 main 中调用）
// ctx: 全局 NPCContext
void difftest_init(NPCContext* ctx);

// 单步差分测试（每条指令后调用）
// ctx: 当前 NPCContext
void difftest_step(NPCContext* ctx);

#endif
