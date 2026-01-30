#include "common.h"

//定义全局上下文
NPCContext npc_ctx = {
    .pc = 0,
    .inst = 0,
    .cycles = 0,
    .stop = false,
    .stop_reason = nullptr,
    .entry = 0,
    .a0_value = 0,
    .debug = {
        .sdb_enabled = false,
        .sdb_step_count = 0,
        .watchpoint_count = 0,
        .regs = {0},
        .difftest_enabled = false,
        .itrace_enabled = false,
        .mtrace_enabled = false,
        .ftrace_enabled = false,
        .cycle_count = 0,
        .hit_good_trap = false,
        .hit_bad_trap = false,
        .invalid_count = 0
    }
};