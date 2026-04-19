#ifndef ITRACE_H
#define ITRACE_H

#include "common.h"

//初始化LLVM反汇编器上下文并清空环形缓冲区
void itrace_init(NPCContext* ctx);


//单步指令追踪，读取ctx中的当前pc/inst并打印反汇编结果
void itrace_step(NPCContext* ctx);


//反汇编并打印一条指定指令
void itrace_disassemble(uint64_t pc, uint32_t inst);


//打印最近记录的指令环形缓冲区(失败诊断)
void itrace_dump_iringbuf(void);

#endif
