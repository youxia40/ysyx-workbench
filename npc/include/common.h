#ifndef COMMON_H
#define COMMON_H

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cstddef>
#include <string>



#define NPC_ENABLE_SDB      1 
#define NPC_ENABLE_DIFFTEST 0
#define NPC_ENABLE_ITRACE   0                       //最好结合difftest使用,cpu执行了哪条指令
#define NPC_ENABLE_MTRACE   0                       //访问了哪写地址，读写了什么数据
#define NPC_ENABLE_FTRACE   0

//内存配置
#define MEM_SIZE   (128*1024*1024)           //128MB
#define MEM_BASE   0x80000000
#define MAX_CYCLES 1000000

//mtrace配套
#define MTRACE_ADDR_LO 0x00000000u         //过滤的是访存地址ADDR,不是pc……
#define MTRACE_ADDR_HI 0xffffffffu
#define MTRACE_LOG_READ  0
#define MTRACE_LOG_WRITE 1
#define MTRACE_MAX_LINES 100
//ftrace
#define FTRACE_MAX_LINES 100



//寄存器数(RV32E）
#define REG_NUM 16

//寄存器名
static const char* reg_names[REG_NUM] = {
    "zero", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
    "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5"
};

//调试状态
typedef struct {
    bool sdb_enabled;
    int sdb_step_count;
    int watchpoint_count;
    uint32_t regs[REG_NUM];
    bool difftest_enabled;
    bool itrace_enabled;
    bool mtrace_enabled;
    bool ftrace_enabled;
    uint64_t cycle_count;
    bool hit_good_trap;
    bool hit_bad_trap;
    int invalid_count;
} DebugContext;
typedef struct {
    uint32_t pc;
    uint32_t inst;
    uint64_t cycles;
    bool stop;
    const char* stop_reason;
    uint32_t entry;
    uint32_t a0_value;
    DebugContext debug;

    char elf_path[256];                 //让ftrace知道ELF在哪
} NPCContext;




extern NPCContext npc_ctx;

#endif