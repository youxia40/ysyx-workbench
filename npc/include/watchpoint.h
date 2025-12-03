#ifndef WATCHPOINT_H
#define WATCHPOINT_H

#include "common.h"

// 监视点结构
typedef struct {
    int id;
    char expr[128];
    uint32_t value;
} Watchpoint;

// 初始化监视点池
void init_watchpoints();

// 添加新监视点
Watchpoint* new_watchpoint();

// 删除监视点
void free_watchpoint(int id);

// 列出所有监视点
void list_watchpoints();

// 检查监视点
bool check_watchpoints();

#endif