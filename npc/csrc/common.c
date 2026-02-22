#include "common.h"//全局上下文定义,统一保存NPC运行态与调试态

//定义全局上下文
NPCContext npc_ctx = {
    .pc = 0,//当前提交到监视器的PC
    .inst = 0,//当前提交到监视器的指令
    .cycles = 0,//总执行周期计数
    .stop = false,//停止标记
    .stop_reason = nullptr,//停止原因字符串
    .entry = 0,//镜像入口地址
    .a0_value = 0,//用于GOODTRAP/BADTRAP判定的返回码
    .debug = {
        .sdb_enabled = false,//是否进入交互调试
        .sdb_step_count = 0,//剩余单步指令数
        .regs = {0},//寄存器镜像快照，用来支持查看寄存器
        .difftest_enabled = false,//差分测试开关
        .itrace_enabled = false,//指令追踪开关
        .mtrace_enabled = false,//访存追踪开关
        .ftrace_enabled = false,//函数追踪开关
        .cycle_count = 0,//调试视角周期计数
        .hit_good_trap = false,//是否命中GOODTRAP
        .hit_bad_trap = false,//是否命中BADTRAP
        .invalid_count = 0//连续非法指令计数
    }
    //未显式初始化某模块也有起始行为
};