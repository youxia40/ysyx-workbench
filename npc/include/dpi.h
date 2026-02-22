#ifndef DPI_H
#define DPI_H

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

void npc_ebreak(uint32_t pc);//处理ebreak陷阱并写入停止原因
int  npc_pmem_read(uint32_t addr);//供RTL读取物理内存或MMIO数据
void npc_pmem_write(uint32_t addr, uint32_t data, uint8_t mask);//供RTL按字节掩码写物理内存或MMIO
void npc_get_regs(uint32_t regs[REG_NUM]);//导出通用寄存器用于调试/比对
void npc_set_reg(int idx, uint32_t value);//同步RTL写回后的寄存器值
void npc_itrace_log(uint64_t pc, uint32_t inst);//指令追踪日志钩子
void npc_mtrace_log(int is_read, uint32_t addr, int len, uint32_t data);//访存追踪日志钩子
void npc_ftrace_log(uint64_t pc, uint64_t target_pc, int is_call);//函数调用追踪日志钩子

#ifdef __cplusplus
}
#endif

#endif
