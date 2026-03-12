#include <am.h>
#include <riscv/riscv.h>
#include <klib.h>

#define MCAUSE_ECALL_UMODE 8
#define MCAUSE_ECALL_SMODE 9
#define MCAUSE_ECALL_MMODE 11
#define MCAUSE_IRQ_TIMER 0x80000007u
#define MCAUSE_IRQ_IODEV 0x8000000bu
/*
struct Context {//处理器上下文结构体
  uintptr_t gpr[NR_REGS];
  uintptr_t mcause, mstatus, mepc;
  void *pdir;//地址空间根指针
};
*/
static Context* (*user_handler)(Event, Context*) = NULL;//事件处理函数指针，AM在发生异常时会调用它来处理事件
/*
mcause值 → 事件一览（riscv32）
异常（bit31=0）（执行指令时触发）：
  mcause十进制	mcause十六进制	事件（Cause）
  0	           0x00000000	    Instruction address misaligned（取指地址未对齐）
  1	           0x00000001	    Instruction access fault（取指访问错误）
  2	           0x00000002	    Illegal instruction（非法指令）
  3	           0x00000003	    Breakpoint（断点）
  4            0x00000004	    Load address misaligned（Load 地址未对齐）
  5            0x00000005	    Load access fault（Load 访问错误）
  6	           0x00000006	    Store/AMO address misaligned（Store/AMO 地址未对齐）
  7	           0x00000007	    Store/AMO access fault（Store/AMO 访问错误）
  8	           0x00000008	    Environment call from U-mode（U 模式 ecall）
  9	           0x00000009	    Environment call from S-mode（S 模式 ecall）
  11	         0x0000000b	    Environment call from M-mode（M 模式 ecall）
  12	         0x0000000c	    Instruction page fault（取指页故障，若有分页）
  13	         0x0000000d	    Load page fault（Load 页故障，若有分页）
  15	         0x0000000f	    Store/AMO page fault（Store/AMO 页故障，若有分页）

中断（bit31=1）（外设/定时器触发）：
mcause十进制（含bit31）	mcause十六进制	事件（Cause）
2147483651	          0x80000003	   Machine software interrupt（机器软件中断 MSIP）
2147483655	          0x80000007	   Machine timer interrupt（机器定时器中断 MTIP）
2147483659	          0x8000000b	   Machine external interrupt（机器外部中断 MEIP）
*/

Context* __am_irq_handle(Context *c) {//异常处理函数（事件分发），AM在发生异常时会调用它来处理事件，参数c是当前的处理器上下文
//在/home/pz40/ysyx-workbench/abstract-machine/am/src/riscv/nemu/trap.S调用
//__am_asm_trap函数负责保存现场并调用__am_irq_handle处理事件

  if (user_handler) {//事件处理函数指针不为空
    Event ev = {//初始化事件结构体，默认事件类型为EVENT_NULL
      .event = EVENT_NULL,
      .cause = c->mcause,
      .ref = c->mepc,
      .msg = "unknown trap",
    };

    switch (c->mcause) {//根据mcause寄存器的值来判断异常类型，并设置事件类型
      case MCAUSE_IRQ_TIMER:
        ev.event = EVENT_IRQ_TIMER;
        ev.msg = "machine timer interrupt";
        break;
      case MCAUSE_IRQ_IODEV:
        ev.event = EVENT_IRQ_IODEV;
        ev.msg = "machine external interrupt";
        break;
      case MCAUSE_ECALL_UMODE:
      case MCAUSE_ECALL_SMODE:
      case MCAUSE_ECALL_MMODE:
        c->mepc += 4;//跳过ecall指令，防止重复触发同一事件
        ev.ref = c->mepc;//更新ref为下一条指令地址，方便进行异常返回
        if ((intptr_t)c->GPR1 == -1) {//当a7/a5寄存器为-1时表示触发自陷事件，交由调度器处理；否则表示正常系统调用事件，交由操作系统进行处理
          //GPR1于riscv.h中定义
          ev.event = EVENT_YIELD;
          ev.msg = "yield ecall";
        } else {
          ev.event = EVENT_SYSCALL;
          ev.msg = "syscall ecall";
        }
        break;

      default:
        ev.event = EVENT_ERROR;
        ev.msg = "unhandled trap";
        break;
    }

    c = user_handler(ev, c);//调用事件处理函数，传入事件和当前上下文，得到新的上下文
    assert(c != NULL);
  }

  return c;//返回新的上下文，c就是“当前异常栈帧首地址”，trap.S会根据这个地址恢复寄存器并执行mret返回到用户程序
}//恢复寄存器与mret返回本质在trap.S完成


extern void __am_asm_trap(void);//异常入口函数，定义在trap.S中，负责保存现场并调用__am_irq_handle处理事件

bool cte_init(Context*(*handler)(Event, Context*)) {//CTE初始化函数，设置异常入口和事件处理函数
  // initialize exception entry初始化异常条目
  asm volatile("csrw mtvec, %0" : : "r"(__am_asm_trap));//把异常入口地址写入mtvec寄存器，确保发生异常时CPU会跳转到__am_asm_trap函数

  // register event handler注册事件处理回调函数，AM在发生事件时会调用这个函数来处理事件
  user_handler = handler;

  return true;
}

Context *kcontext(Area kstack, void (*entry)(void *), void *arg) {
  //创建内核线程的上下文，kstack是线程的内核栈区域范围，entry是线程的入口函数，arg是传递给入口函数的参数，是创建初始Context的接口
  //用来创建一个新的上下文，初始状态是当这个上下文被切换到时会执行entry函数，并且传入arg作为参数，指定了这个上下文使用的内核栈范围。

  Context *c = (Context *)((uintptr_t)kstack.end - sizeof(Context));//在内核栈的顶部为新的上下文结构预留空间，确保上下文结构位于栈顶，符合调用约定
  *c = (Context){0};

  uintptr_t mstatus = 0;
  asm volatile("csrr %0, mstatus" : "=r"(mstatus));
  //从mstatus寄存器读取当前的处理器状态，存储在变量mstatus中，这样我们就能在新上下文中继承当前的处理器状态，例如中断使能等

  c->mstatus = mstatus;
  c->mepc = (uintptr_t)entry;
  c->pdir = NULL;
  c->gpr[10] = (uintptr_t)arg;//RISC-V：a0寄存器传递第一个参数

  return c;
}

void yield() {//触发自陷事件的函数，调用后会触发一个编号为EVENT_YIELD的事件，交由操作系统调度器处理
#ifdef __riscv_e
  asm volatile("li a5, -1; ecall");
#else
  asm volatile("li a7, -1; ecall");
  //在RISC-V中，a7寄存器通常用于传递系统调用号，这里把它设置为-1表示触发一个特殊的自陷事件，操作系统调度器会检测到这个事件并进行相应的处理，例如切换到另一个线程或进程。
#endif
}

bool ienabled() {//检查中断是否使能，通过读取mstatus寄存器的值来判断中断使能位（通常是第3位）是否被设置，如果设置了则返回true，表示中断已使能，否则返回false。
  uintptr_t mstatus = 0;
  asm volatile("csrr %0, mstatus" : "=r"(mstatus));//从mstatus寄存器读取当前的处理器状态，存储在变量mstatus中
  return (mstatus & (1u << 3)) != 0;
}

void iset(bool enable) {//设置中断使能状态，设置或清除mstatus寄存器中的中断使能位（通常是第3位）。如果enable为true，则使用csrsi指令设置该位；如果enable为false，则使用csrci指令清除该位。
  if (enable) {
    asm volatile("csrsi mstatus, 8");
  } else {
    asm volatile("csrci mstatus, 8");
  }
}
