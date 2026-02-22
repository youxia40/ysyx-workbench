#ifndef SDB_H
#define SDB_H

#include "common.h"


void sdb_init(NPCContext *ctx);//初始化调试器与监视点系统

//单步执行n条指令
void sdb_step(NPCContext *ctx);//执行一条调试命令

//继续运行
void sdb_cont(NPCContext *ctx);//继续运行直到停止条件触发

//打印寄存器
void sdb_regs(void);//打印全部寄存器值

//扫描内存，从addr开始读n个
void sdb_x(int n, uint32_t addr);//从指定地址开始查看内存

//检查监视点
void sdb_wpchk(NPCContext *ctx);//检查监视点是否命中

#endif
