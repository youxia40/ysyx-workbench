#ifndef SDB_H
#define SDB_H

#include "sim_core.h"  // 包含SimContext定义
#include <cstdint>
#include <string>
#include <vector>
#include <map>

// 监视点结构体
struct Watchpoint {
    int id;                     // 监视点ID
    std::string expr;           // 表达式（如"x1"或"0x1000"）
    uint32_t last_value;        // 上次值
};

// SDB函数声明（与sdb.cpp实现一致）
void init_sdb(SimContext* sim);
void sdb_mainloop(SimContext* sim, int max_cycles);
uint32_t expr_eval(const char* expr); // 表达式求值
int add_watchpoint(const char* expr); // 添加监视点
bool delete_watchpoint(int id);       // 删除监视点
void list_watchpoints();              // 列出监视点
void check_watchpoints();             // 检查监视点触发

#endif