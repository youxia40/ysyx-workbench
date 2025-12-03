#ifndef MTRACE_H
#define MTRACE_H

#include "common.h"

// 记录一次内存访问（由 C 侧 / DPI 调用）
void mtrace_log(int is_read, uint32_t addr, int len, uint32_t data);

// 每拍调用的 hook（目前留空，实现都在 mtrace_log 里）
void mtrace_step(NPCContext* ctx);

#endif
