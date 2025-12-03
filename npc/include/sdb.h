#ifndef SDB_H
#define SDB_H

#include "common.h"

// 初始化SDB调试器
void sdb_init(NPCContext* ctx);

// 执行单步调试
void sdb_step(NPCContext* ctx);

// 添加监视点
void sdb_add_watchpoint(const char* expr);

// 删除监视点
void sdb_delete_watchpoint(int id);

// 列出所有监视点
void sdb_list_watchpoints();

// 打印寄存器值
void sdb_print_registers();

// 扫描内存内容
void sdb_scan_memory(uint32_t addr);

// 执行"c"命令（继续运行）
void sdb_continue_execution(NPCContext* ctx);

// 检查监视点
void sdb_check_watchpoints(NPCContext* ctx);

#endif