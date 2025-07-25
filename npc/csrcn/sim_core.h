#ifndef SIM_CORE_H
#define SIM_CORE_H

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#define PMEM_BASE 0x80000000
#define PMEM_SIZE (128 * 1024 * 1024)  // 128MB
#define PMEM_END  0x87FFFFFF

#define PC_UPDATE_LOCK // 锁定PC更新，避免并发修改

#define EXEC_HISTORY_SIZE 10
struct ExecutionRecord {
    uint32_t pc;
    uint32_t inst;
    uint8_t opcode;
};

#ifdef __cplusplus
extern "C" {
#endif

struct SimContext {
    uint8_t* memory;        // 内存数组（64MB）
    uint32_t mem_size;      // 内存大小（64MB）
    uint32_t regs[16];  // 统一寄存器数组
    uint32_t pc;            // 程序计数器（PC）
    uint64_t total_instructions; // 总执行指令数
    bool hit_good_trap;     // 是否触发GOOD TRAP（如EBREAK）
    bool hit_bad_trap;      // 是否触发BAD TRAP（如非法指令）
    bool stop;              // 仿真停止标志
    const char* program_path; // 加载的程序路径
    uint32_t program_size;  // 程序大小
    const char* elf_file;   // ELF文件路径（可选）
    bool diff_test_enabled; // 是否启用DiffTest
    void* nemu_handle;      // NEMU共享库句柄

    int max_cycles;         // 最大执行周期（用于SDB）
    bool sdb_exit_requested; // SDB退出请求标志

    struct ExecutionRecord history[EXEC_HISTORY_SIZE];
    uint8_t history_idx;
    bool reg_write_enabled[16]; // 标记寄存器是否被显式写入



    // 新增 PC 修改追踪
    uint32_t last_valid_pc;          // 最后一次有效 PC
    uint32_t pc_modifier;            // 最后修改 PC 的模块标识
    uint32_t pc_modification_count;  // PC 修改次数计数器

    

    #ifdef PC_UPDATE_LOCK
    uint32_t pc_lock;  // PC 更新锁（0=解锁，1=锁定）
    #endif
};

// 函数声明
SimContext* get_sim_context();
void enable_diff_test(SimContext* sim, bool enable);
SimContext* sim_init();
void sim_free(SimContext* sim);
int sim_load_program(SimContext* sim, const char* program_path);
int pmem_read(int raddr);
void pmem_write(int waddr, int wdata, char wmask);
void get_regs(uint32_t regs[32]);
void set_regs(uint32_t regs[32]);
void safe_set_pc(SimContext* sim, uint32_t new_pc);
void print_stack_trace(SimContext* sim);
void record_execution(SimContext* sim, uint32_t pc, uint32_t inst);
void print_execution_history(SimContext* sim);

#ifdef __cplusplus
}
#endif

#endif