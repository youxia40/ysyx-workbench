#ifndef DPI_H
#define DPI_H

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif


//DPI-C函数
void npc_ebreak(uint32_t pc);
int  npc_pmem_read(uint32_t addr);
void npc_pmem_write(uint32_t addr, uint32_t data, uint8_t mask);


//从NPC上下文获取一份GPR快照
void npc_get_regs(uint32_t regs[16]);


//RTL每次写寄存器时，把GPR写回到npc_ctx.debug.regs中
void npc_set_reg(int idx, uint32_t value);



//trace相关
void npc_itrace_log(uint64_t pc, uint32_t inst);
void npc_mtrace_log(int is_read, uint32_t addr, int len, uint32_t data);
void npc_ftrace_log(uint64_t pc, uint64_t target_pc, int is_call);

#ifdef __cplusplus
}
#endif

#endif
