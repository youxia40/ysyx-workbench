#ifndef DIFFTEST_H
#define DIFFTEST_H

#include "sim_core.h"  // 包含SimContext定义

// DiffTest状态枚举
typedef enum {
    DIFFTEST_PASS,   // 测试通过
    DIFFTEST_FAIL,   // 测试失败
    DIFFTEST_ABORT   // 测试终止
} DiffTestResult;

// 初始化DiffTest（加载NEMU共享库，获取函数指针）
bool init_diff_test(SimContext* sim);

// 执行单步DiffTest（对比NPC与NEMU状态）
bool do_diff_test(SimContext* sim);

#endif