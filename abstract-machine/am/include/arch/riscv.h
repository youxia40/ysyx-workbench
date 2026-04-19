#ifndef ARCH_H__
#define ARCH_H__

#ifdef __riscv_e
#define NR_REGS 16
#else
#define NR_REGS 32
#endif


//Context不是“临时struct拷贝”，而是trap.S在栈上按约定布局直接构造出来的一块内存
//不是先构造再拷贝，而是trap.S在栈上按布局直接“摆”出来，然后C代码把这块内存当Context*解读,布局由trap.S的offset宏落实
struct Context {//处理器上下文结构体
  // TODO: fix the order of these members to match trap.S
  uintptr_t gpr[NR_REGS];//通用寄存器镜像，对应/home/pz40/ysyx-workbench/abstract-machine/am/src/riscv/nemu/trap.S先保存x寄存器
  uintptr_t mcause, mstatus, mepc;
  //mstatus寄存器 - 存放处理器的状态，在nemu/src/isa/riscv32/init.c将mstatus初始化为0x1800


  void *pdir;//地址空间根指针
  //riscv/nemu/trap.S将CONTEXT_SIZE设置为((NR_REGS + 4) * XLEN)，以确保pdir成员被正确保存和恢复
};

#ifdef __riscv_e
#define GPR1 gpr[15] // a5
#else
#define GPR1 gpr[17] // a7
#endif

#define GPR2 gpr[0]
#define GPR3 gpr[0]
#define GPR4 gpr[0]
#define GPRx gpr[0]

#endif
