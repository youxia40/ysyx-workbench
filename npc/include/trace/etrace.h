#ifndef ETRACE_H
#define ETRACE_H

#include "common.h"

void etrace_trap(uint32_t no, uint32_t epc, uint32_t handler);
void etrace_mret(uint32_t from, uint32_t to);

#endif
