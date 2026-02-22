#ifndef MEM_H
#define MEM_H

#include "common.h"

int  npc_mem_read(uint32_t addr);//按word读取模拟内存
void npc_mem_write(uint32_t addr, uint32_t data, uint8_t mask);//按字节掩码写模拟内存
void npc_get_registers(uint32_t regs[REG_NUM]);//读取当前寄存器镜像

#endif
