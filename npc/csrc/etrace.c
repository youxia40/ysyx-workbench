#include "etrace.h"

#if NPC_ENABLE_ETRACE

void etrace_trap(uint32_t no, uint32_t epc, uint32_t handler) {
  const char *type = ((no >> 31) & 1u) ? "IRQ" : "EXC";
  printf("\n[etrace] trap %-3s no=0x%08x epc=0x%08x -> 0x%08x\n", type, no, epc, handler);
}

void etrace_mret(uint32_t from, uint32_t to) {
  printf("[etrace] mret pc=0x%08x -> 0x%08x\n", from, to);
}

#else

void etrace_trap(uint32_t no, uint32_t epc, uint32_t handler) {
  (void)no;
  (void)epc;
  (void)handler;
}

void etrace_mret(uint32_t from, uint32_t to) {
  (void)from;
  (void)to;
}

#endif
