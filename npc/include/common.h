#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>


#define NPC_ENABLE_ASSERT   0//断言
#if NPC_ENABLE_ASSERT
#include <assert.h>
#endif

#define NPC_ENABLE_SDB      1//交互调试
#define NPC_ENABLE_DIFFTEST 1//差分测试
#define NPC_ENABLE_ITRACE   0//指令追踪
#define NPC_ENABLE_MTRACE   0//访存追踪
#define NPC_ENABLE_FTRACE   0//函数追踪
#define NPC_ENABLE_WAVE     0//波形


//内存配置
#define MEM_SIZE   (128*1024*1024)//主存总大小
#define MEM_BASE   0x80000000//主存起始地址
#define MAX_CYCLES 1000000000//仿真最大周期保护阈值(大型程序需更高上限)

//mtrace配套
#define MTRACE_ADDR_LO 0x00000000u//访存地址过滤下限
#define MTRACE_ADDR_HI 0xffffffffu//访存地址过滤上限
#define MTRACE_LOG_READ  0//是否记录读访存
#define MTRACE_LOG_WRITE 1//是否记录写访存
#define MTRACE_MAX_LINES 100//最大输出行数
//ftrace
#define FTRACE_MAX_LINES 100//最大输出行数



//寄存器数(RV32E）
#define REG_NUM 16

//寄存器名
static const char* reg_names[REG_NUM] = {
    "zero", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
    "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5"
};

//调试状态
#ifdef __cplusplus
extern "C" {
#endif
typedef struct {//调试相关状态,用于sdb/trace/trap判定
    int sdb_enabled;//是否允许进入SDB交互
    int sdb_step_count;//剩余单步执行条数
    uint32_t regs[REG_NUM];//寄存器镜像快照
    int difftest_enabled;//差分测试开关
    int itrace_enabled;//指令追踪开关
    int mtrace_enabled;//访存追踪开关
    int ftrace_enabled;//函数追踪开关
    uint64_t cycle_count;//调试周期计数
    int hit_good_trap;//是否命中GOODTRAP
    int hit_bad_trap;//是否命中BADTRAP
    int invalid_count;//连续非法指令计数
} DebugContext;
typedef struct {//NPC运行时上下文,保存CPU状态与调试信息
    uint32_t pc;//当前提交PC
    uint32_t inst;//当前提交指令
    uint64_t cycles;//总执行周期
    int stop;//全局停止标记
    char* stop_reason;//停止原因字符串(静态字面量指针)
    uint32_t entry;//镜像入口地址
    uint32_t a0_value;//用于trap判定的返回码
    DebugContext debug;//调试子状态

    char elf_path[256];//让ftrace知道ELF路径
} NPCContext;

extern NPCContext npc_ctx;

#ifdef __cplusplus
}
#endif
#endif