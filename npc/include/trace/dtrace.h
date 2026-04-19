#ifndef DTRACE_H
#define DTRACE_H

#include "common.h"

void dtrace_enable(bool enable);
void dtrace_read(uint32_t addr, int len, const char *dev);
void dtrace_write(uint32_t addr, int len, uint32_t data, const char *dev);

#endif
