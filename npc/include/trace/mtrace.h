#ifndef MTRACE_H
#define MTRACE_H

#include "common.h"


//记录一次内存访问
void mtrace_log(int is_read, uint32_t addr, int len, uint32_t data);//记录一次访存事件


#endif
