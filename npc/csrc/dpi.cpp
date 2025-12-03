#include "common.h"
#include "dpi.h"
#include "mem.h"
#include "itrace.h"
#include "mtrace.h"
#include "ftrace.h"
#include <cstdio>

// 使用全局上下文
extern NPCContext npc_ctx;

// 物理内存读操作（paddr 是相对 MEM_BASE 的偏移）
int npc_pmem_read(uint32_t paddr) {
  uint32_t vaddr = MEM_BASE + paddr;
  int data = npc_mem_read(vaddr);

#if NPC_ENABLE_MTRACE
  // 读操作，长度目前统一按 4 字节算（你也可以根据总线的 size 再细分）
  npc_mtrace_log(1, vaddr, 4, (uint32_t)data);
#endif

  return data;
}

// 物理内存写操作
void npc_pmem_write(uint32_t paddr, uint32_t data, uint8_t mask) {
  uint32_t vaddr = MEM_BASE + paddr;
  npc_mem_write(vaddr, data, mask);

#if NPC_ENABLE_MTRACE
  // 根据 mask 推断写入长度（针对 RISC-V 常见的掩码形式）
  int len;
  switch (mask) {
    case 0x1: case 0x2: case 0x4: case 0x8:
      len = 1; break;   // sb
    case 0x3: case 0x6: case 0xc:
      len = 2; break;   // sh
    case 0xf:
    default:
      len = 4; break;   // sw
  }

  npc_mtrace_log(0, vaddr, len, data);
#endif
}

// RTL 每次写 GPR 时调用，更新调试用寄存器快照
void npc_set_reg(int idx, uint32_t value) {
  if (idx < 0 || idx >= REG_NUM) return;
  npc_ctx.debug.regs[idx] = value;

  // RV32E 中 a0 是 x10
  if (idx == 10) {
    npc_ctx.a0_value = value;
  }
}

// 从调试上下文中取一份寄存器快照
void npc_get_regs(uint32_t regs[REG_NUM]) {
  for (int i = 0; i < REG_NUM; i++) {
    regs[i] = npc_ctx.debug.regs[i];
  }
}

// 处理 ebreak (nemu_trap)
// 这里根据 a0 的值区分 GOOD/BAD TRAP
void npc_ebreak(uint32_t pc) {
  uint32_t code = npc_ctx.a0_value;

  if (code == 0) {
    printf("[DPI] HIT GOOD TRAP at PC=0x%08X\n", pc);
    npc_ctx.debug.hit_good_trap = true;
  } else {
    printf("[DPI] HIT BAD TRAP at PC=0x%08X, a0 = %u (0x%08X)\n",
           pc, code, code);
    npc_ctx.debug.hit_bad_trap = true;
  }

  npc_ctx.stop = true;
  npc_ctx.pc   = pc;
}

// 指令追踪日志（目前真正的 itrace 在 C 端调用 itrace_step 实现）
void npc_itrace_log(uint64_t pc, uint32_t inst) {
#if NPC_ENABLE_ITRACE
  itrace_disassemble(pc, inst);
#else
  (void)pc; (void)inst;
#endif
}

// 内存访问追踪日志（供 DPI 包装使用）
void npc_mtrace_log(int is_read, uint32_t addr, int len, uint32_t data) {
#if NPC_ENABLE_MTRACE
  mtrace_log(is_read, addr, len, data);
#else
  (void)is_read; (void)addr; (void)len; (void)data;
#endif
}

// 函数调用追踪日志
void npc_ftrace_log(uint64_t pc, uint64_t target_pc, int is_call) {
#if NPC_ENABLE_FTRACE
  ftrace_log(pc, target_pc, is_call);
#else
  (void)pc; (void)target_pc; (void)is_call;
#endif
}