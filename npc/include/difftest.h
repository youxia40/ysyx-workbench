#ifndef DIFFTEST_H
#define DIFFTEST_H

#include "common.h"


void difftest_init(NPCContext* ctx);//初始化参考模型并完成首轮状态同步

void difftest_step(NPCContext* ctx);//每条指令后执行一次状态对比

#endif
