#ifndef DPI_H
#define DPI_H

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

// DPI-C 函数声明（由 Verilog 调用或供 Verilog 导出）
void npc_ebreak(uint32_t pc);
int  npc_pmem_read(uint32_t addr);
void npc_pmem_write(uint32_t addr, uint32_t data, uint8_t mask);

// 从 NPC 上下文获取一份 GPR 快照（供 C 端其它模块使用）
void npc_get_regs(uint32_t regs[16]);

// 由 RTL 在每次写寄存器时调用，用于把 GPR 写回到 npc_ctx.debug.regs 中
void npc_set_reg(int idx, uint32_t value);

// trace 相关
void npc_itrace_log(uint64_t pc, uint32_t inst);
void npc_mtrace_log(int is_read, uint32_t addr, int len, uint32_t data);
void npc_ftrace_log(uint64_t pc, uint64_t target_pc, int is_call);

#ifdef __cplusplus
}
#endif

#endif
