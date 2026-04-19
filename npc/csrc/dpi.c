#include "dpi.h"
#include "mem.h"
#include "mmio.h"
#include "trace/ftrace.h"
#include "trace/itrace.h"
#include "trace/mtrace.h"
#include "trace/etrace.h"

static inline int mask_to_len(uint8_t mask) {
  if (mask == 0x1 || mask == 0x2 || mask == 0x4 || mask == 0x8) {
    return 1;
  }
  if (mask == 0x3 || mask == 0x6 || mask == 0xc) {
    return 2;
  }
  return 4;
}

static inline int mask_to_off(uint8_t mask) {
  if (mask & 0x1) return 0;
  if (mask & 0x2) return 1;
  if (mask & 0x4) return 2;
  if (mask & 0x8) return 3;
  return 0;
}

static inline int mmio_read_len(uint32_t addr) {
  if ((addr & ~0x3u) == (MMIO_SERIAL_ADDR & ~0x3u)) {
    return 1;
  }
  return 4;
}

//把核心给出的地址转换后分发到MMIO或主存
int npc_pmem_read(uint32_t paddr) {
  uint32_t vaddr = MEM_BASE + paddr;//DPI传入的是相对MEM_BASE偏移,先还原虚拟地址
  int data = 0;
#if NPC_ENABLE_ASSERT
  assert((vaddr & 0x3u) == (paddr & 0x3u));//检查访问地址的对齐方式是否正确
#endif

  uint32_t aligned = vaddr & ~0x3u;//统一按word对齐访问后端

  if (mmio_in_range(aligned)) {
    return (int)mmio_read(aligned, mmio_read_len(aligned));//命中设备地址则走设备读通路
  }

  data = npc_mem_read(aligned);

#if NPC_ENABLE_MTRACE
  npc_mtrace_log(1, aligned, 4, (uint32_t)data);//按4字节记录普通主存读事件
#endif
  return data;
}

//根据地址类型分发并按mask写入
void npc_pmem_write(uint32_t paddr, uint32_t data, uint8_t mask) {
  uint32_t vaddr   = MEM_BASE + paddr;//偏移转虚拟地址
  uint32_t aligned = vaddr & ~0x3u;//写入基址按word对齐
  (void)data;
#if NPC_ENABLE_ASSERT
  assert((mask & 0xf0u) == 0);//写掩码只允许低4位
#endif

  if (mmio_in_range(aligned)) {
    int len = mask_to_len(mask);
    int off = mask_to_off(mask);
    uint32_t mmio_addr = aligned + (uint32_t)off;
    uint32_t mmio_data = (len == 4) ? data : (data >> (off * 8));
    mmio_write(mmio_addr, len, mmio_data);//MMIO地址走设备写路径
    return;
  }

  npc_mem_write(aligned, data, mask);//普通地址写回主存

#if NPC_ENABLE_MTRACE
  npc_mtrace_log(0, aligned, mask_to_len(mask), data);
#endif
}

//DPI回写通用寄存器，用于支持ebreak时输出a0值和调试器查看寄存器
void npc_set_reg(int idx, uint32_t value) {//idx是寄存器编号，value是要写入的值
  if (idx < 0 || idx >= REG_NUM) {
    return;
  }
  npc_ctx.debug.regs[idx] = value;
  if (idx == 10) {
    npc_ctx.a0_value = value;//a0
  }
}

//导出寄存器数组给调试器使用
void npc_get_regs(uint32_t regs[REG_NUM]) {
#if NPC_ENABLE_ASSERT
  assert(regs != NULL);
#endif
  for (int i = 0; i < REG_NUM; i++) {
    regs[i] = npc_ctx.debug.regs[i];
  }
}

//ebreak处理，依据a0值区分GOOD TRAP/BAD TRAP并请求停机
void npc_ebreak(uint32_t pc) {
  uint32_t code = npc_ctx.a0_value;//a0约定的返回码

  printf("\n\n");
  if (code == 0) {
    printf("[NPC][DPI] GOODTRAP pc=0x%08X\n", pc);
    npc_ctx.debug.hit_good_trap = true;
  } else {
    printf("[NPC][DPI] BADTRAP pc=0x%08X,a0=%u(0x%08X)\n", pc, code, code);
    npc_ctx.debug.hit_bad_trap = true;
  }

  npc_ctx.stop = true;
  npc_ctx.pc = pc;
}

//DPI-C指令提交回调：把当前提交指令喂给itrace
void npc_itrace_log(uint64_t pc, uint32_t inst) {
  npc_ctx.pc = (uint32_t)pc;
  npc_ctx.inst = inst;
#if NPC_ENABLE_ITRACE
  if (npc_ctx.debug.itrace_enabled) {
    itrace_step(&npc_ctx);
  }
#else
  (void)pc;
  (void)inst;
#endif
}

//DPI-C访存日志回调：受mtrace开关控制
void npc_mtrace_log(int is_read, uint32_t addr, int len, uint32_t data) {
#if NPC_ENABLE_MTRACE
  if (npc_ctx.debug.mtrace_enabled) {
    mtrace_log(is_read, addr, len, data);
  }
#else
  (void)is_read;
  (void)addr;
  (void)len;
  (void)data;
#endif
}

//DPI-C函数追踪回调：受ftrace开关控制
void npc_ftrace_log(uint64_t pc, uint64_t target_pc, int is_call) {
#if NPC_ENABLE_FTRACE
  if (npc_ctx.debug.ftrace_enabled) {
    ftrace_log(pc, target_pc, is_call);
  }
#else
  (void)pc;
  (void)target_pc;
  (void)is_call;
#endif
}

void npc_etrace_trap(uint32_t no, uint32_t epc, uint32_t handler) {
#if NPC_ENABLE_ETRACE
  if (npc_ctx.debug.etrace_enabled) {
    etrace_trap(no, epc, handler);
  }
#else
  (void)no;
  (void)epc;
  (void)handler;
#endif
}

void npc_etrace_mret(uint32_t from, uint32_t to) {
#if NPC_ENABLE_ETRACE
  if (npc_ctx.debug.etrace_enabled) {
    etrace_mret(from, to);
  }
#else
  (void)from;
  (void)to;
#endif
}
