#ifndef MTRACE_H
#define MTRACE_H

#include "common.h"


//记录一次内存访问
void mtrace_log(int is_read, uint32_t addr, int len, uint32_t data);


//每次调用的hook，实现都在mtrace_log里
void mtrace_step(NPCContext* ctx);

#endif
