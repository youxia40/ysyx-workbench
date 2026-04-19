#include "trace/etrace.h"

#if NPC_ENABLE_ETRACE

//FMT等格式化宏定义在common.h中，用于打印不同类型的格式化字符串
void etrace_trap(uint32_t no, uint32_t epc, uint32_t handler) {
  //记录发生的中断/异常信息，是陷入异常时从“mepc”跳到“异常入口地址__am_asm_trap”
  const char *type = ((no >> 31) & 1u) ? "IRQ" : "EXC";
  printf("\n[etrace] trap %-3s no=0x%08x epc=0x%08x -> 0x%08x\n", type, no, epc, handler);
}

void etrace_mret(uint32_t from, uint32_t to) {
  //记录从异常返回的信息，是从mret跳到“mepc的下一条指令ret”
  printf("[etrace] mret pc=0x%08x -> 0x%08x\n", from, to);
}

#else

//trap事件追踪
void etrace_trap(uint32_t no, uint32_t epc, uint32_t handler) {
  (void)no;
  (void)epc;
  (void)handler;
}

//mret事件追踪
void etrace_mret(uint32_t from, uint32_t to) {
  (void)from;
  (void)to;
}

#endif
