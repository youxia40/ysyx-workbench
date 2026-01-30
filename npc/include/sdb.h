#ifndef SDB_H
#define SDB_H

#include "common.h"


void sdb_init(NPCContext *ctx);

//单步执行n条指令
void sdb_step(NPCContext *ctx);

//继续运行
void sdb_cont(NPCContext *ctx);

//打印寄存器
void sdb_regs(void);

//扫描内存，从addr开始读n个
void sdb_x(int n, uint32_t addr);

//检查监视点
void sdb_wpchk(NPCContext *ctx);

#endif
