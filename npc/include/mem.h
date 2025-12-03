#ifndef MEM_H
#define MEM_H

#include "common.h"

int npc_mem_read(uint32_t addr);
void npc_mem_write(uint32_t addr, uint32_t data, uint8_t mask);
void npc_get_registers(uint32_t regs[REG_NUM]);

#endif