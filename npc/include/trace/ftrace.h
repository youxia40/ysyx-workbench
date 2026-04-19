#ifndef FTRACE_H
#define FTRACE_H

#include "common.h"


void ftrace_init(NPCContext* ctx);//读取ELF符号并初始化函数表


//记录一次函数调用/返回
//pc:当前指令的PC
//target_pc:对于调用,目标为PC
//is_call:1为CALL,0为RET
void ftrace_log(uint64_t pc, uint64_t target_pc, int is_call);//记录一次调用或返回事件


#endif
