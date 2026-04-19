#define concat_temp(x, y) x ## y
#define concat(x, y) concat_temp(x, y)
#define MAP(c, f) c(f)

#if __riscv_xlen == 32
#define LOAD  lw
#define STORE sw
#define XLEN  4
#else
#define LOAD  ld
#define STORE sd
#define XLEN  8
#endif

#define REGS_LO16(f) \
      f( 1)       f( 3) f( 4) f( 5) f( 6) f( 7) f( 8) f( 9) \
f(10) f(11) f(12) f(13) f(14) f(15)
#ifndef __riscv_e
#define REGS_HI16(f) \
                                    f(16) f(17) f(18) f(19) \
f(20) f(21) f(22) f(23) f(24) f(25) f(26) f(27) f(28) f(29) \
f(30) f(31)
#define NR_REGS 32
#else
#define REGS_HI16(f)
#define NR_REGS 16
#endif

#define REGS(f) REGS_LO16(f) REGS_HI16(f)

#define PUSH(n) STORE concat(x, n), (n * XLEN)(sp);
/*PUSH(n)把各个xN存到sp+n*XLEN，所以上下文起始区域天然就是一整段连续的通用寄存器*/
/*故riscv.h的结构体中gpr优先*/

#define POP(n)  LOAD  concat(x, n), (n * XLEN)(sp);

#define CONTEXT_SIZE  ((NR_REGS + 4) * XLEN)
#define OFFSET_SP     ( 2 * XLEN)
#define OFFSET_CAUSE  ((NR_REGS + 0) * XLEN)
#define OFFSET_STATUS ((NR_REGS + 1) * XLEN)
#define OFFSET_EPC    ((NR_REGS + 2) * XLEN)
#define OFFSET_PDIR   ((NR_REGS + 3) * XLEN)

.align 3
.globl __am_asm_trap
/*异常入口函数负责保存现场并调用__am_irq_handle处理事件*/

__am_asm_trap:
  addi sp, sp, -CONTEXT_SIZE

  /*写入gpr寄存器*/
  MAP(REGS, PUSH)

  /*单独把“陷入前的原始sp”写到gpr[2]*/
  addi t0, sp, CONTEXT_SIZE
  STORE t0, OFFSET_SP(sp)

  /*读CSR并写入上下文*/
  csrr t0, mcause
  csrr t1, mstatus
  csrr t2, mepc


  STORE t0, OFFSET_CAUSE(sp)
  STORE t1, OFFSET_STATUS(sp)
  STORE t2, OFFSET_EPC(sp)
  STORE zero, OFFSET_PDIR(sp)
  /*pdir先置0，只保证字段合法可读*/

  # set mstatus.MPRV to pass difftest
  li a0, (1 << 17)
  or t1, t1, a0
  csrw mstatus, t1


  /*把当前Context*传给C处理*/
  mv a0, sp
  /*先保存现场并把sp即（Context*）放到a0*/
  call __am_irq_handle
  /*处理事件*/


  mv sp, a0
  /*将栈顶指针切换到新进程的上下文结构*/
  /*使用__am_irq_handle返回的Context*作为恢复基址，支持调度切换。恢复阶段按返回的Context指针执行，这样user_handler返回新上下文时就会切过去*/


  /*从当前要恢复的Context里取回CSR值*/
  LOAD t1, OFFSET_STATUS(sp)
  LOAD t2, OFFSET_EPC(sp)
  

  /*恢复异常状态*/
  csrw mstatus, t1
  /*恢复异常地址*/
  csrw mepc, t2
  /*恢复x寄存器*/
  MAP(REGS, POP)

  /*恢复上下文*/
  addi sp, sp, CONTEXT_SIZE
  mret
