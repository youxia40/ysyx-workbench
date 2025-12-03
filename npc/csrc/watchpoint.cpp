#include "watchpoint.h"
#include "common.h"
#include "expr.h"
#include <vector>
#include <cstdio>

#define MAX_WATCHPOINTS 32

static std::vector<Watchpoint> watchpoints;
static int next_id = 1;

// 初始化监视点池
void init_watchpoints() {
    watchpoints.clear();
    next_id = 1;
}

// 添加新监视点
Watchpoint* new_watchpoint() {
    if (watchpoints.size() >= MAX_WATCHPOINTS) {
        printf("No more watchpoints available\n");
        return NULL;
    }
    
    Watchpoint wp;
    wp.id = next_id++;
    wp.expr[0] = '\0'; // 初始化表达式
    wp.value = 0;      // 初始化值
    watchpoints.push_back(wp);
    return &watchpoints.back();
}

// 删除监视点
void free_watchpoint(int id) {
    for (auto it = watchpoints.begin(); it != watchpoints.end(); ++it) {
        if (it->id == id) {
            watchpoints.erase(it);
            return;
        }
    }
    printf("Watchpoint %d not found\n", id);
}

// 列出所有监视点
void list_watchpoints() {
    if (watchpoints.empty()) {
        printf("No watchpoints\n");
        return;
    }
    
    printf("ID\tExpression\tValue\n");
    for (const auto& wp : watchpoints) {
        printf("%d\t%s\t0x%08X\n", wp.id, wp.expr, wp.value);
    }
}

// 检查监视点
bool check_watchpoints() {
    bool triggered = false;
    for (auto& wp : watchpoints) {
        bool success = false;
        uint32_t new_value = expr_eval(wp.expr, &success);
        if (success && new_value != wp.value) {
            printf("Watchpoint %d triggered: %s changed from 0x%08X to 0x%08X\n",
                   wp.id, wp.expr, wp.value, new_value);
            wp.value = new_value;
            triggered = true;
        }
    }
    return triggered;
}