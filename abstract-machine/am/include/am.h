#ifndef AM_H__
#define AM_H__

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include ARCH_H // this macro is defined in $CFLAGS
                // examples: "arch/x86-qemu.h", "arch/native.h", ...

// Memory protection flags
#define MMAP_NONE  0x00000000 // no access
#define MMAP_READ  0x00000001 // can read
#define MMAP_WRITE 0x00000002 // can write

// Memory area for [@start, @end)
typedef struct {
  void *start, *end;
} Area;

// Arch-dependent processor context
typedef struct Context Context;//上下文结构体，包含寄存器状态等信息，具体定义由不同架构实现

// An event of type @event, caused by @cause of pointer @ref
typedef struct {//统一描述“为什么切换执行流”
  enum {
    EVENT_NULL = 0,//空事件，表示无特定事件
    EVENT_YIELD, //调度事件，表示当前线程主动放弃CPU执行权，交由调度器选择下一个线程运行
    EVENT_SYSCALL, //系统调用事件，表示用户程序发起系统调用请求，需要操作系统进行处理
    EVENT_PAGEFAULT, //页错误事件，表示访问了无效的内存地址或权限不足的内存区域，操作系统需要进行异常处理
    EVENT_ERROR,//一般错误事件，表示发生了未分类的错误情况，操作系统需要进行错误处理
    EVENT_IRQ_TIMER, //定时器中断事件，表示定时器设备触发了中断，操作系统需要进行相应的处理
    EVENT_IRQ_IODEV,//设备中断事件，表示其他I/O设备触发了中断，操作系统需要进行相应的处理
    
  } event;//事件类型枚举

  
  //PA暂不主要用
  uintptr_t cause, ref;//事件相关的原因码和指针，具体含义根据事件类型而定，例如页错误可能包含访问地址，设备中断可能包含设备ID等
  const char *msg;//事件相关消息字符串，用于错误描述
} Event;

// A protected address space with user memory @area
// and arch-dependent @ptr
typedef struct {//地址空间结构体，包含页大小、用户内存区域和架构相关指针
  int pgsize;
  Area area;
  void *ptr;
} AddrSpace;

#ifdef __cplusplus
extern "C" {
#endif

// ----------------------- TRM: Turing Machine -----------------------
extern   Area        heap;
void     putch       (char ch);
void     halt        (int code) __attribute__((__noreturn__));

// -------------------- IOE: Input/Output Devices --------------------
bool     ioe_init    (void);
void     ioe_read    (int reg, void *buf);
void     ioe_write   (int reg, void *buf);
#include "amdev.h"

// ---------- CTE: Interrupt Handling and Context Switching ----------
bool     cte_init    (Context *(*handler)(Event ev, Context *ctx));
//进行CTE相关的初始化操作.其中它还接受一个来自操作系统的事件处理回调函数的指针, 当发生事件时, CTE将会把事件和相关的上下文作为参数, 来调用这个回调函数, 交由操作系统进行后续处理.

void     yield       (void);//用于进行自陷操作, 会触发一个编号为EVENT_YIELD事件
bool     ienabled    (void);
void     iset        (bool enable);
Context *kcontext    (Area kstack, void (*entry)(void *), void *arg);

// ----------------------- VME: Virtual Memory -----------------------
bool     vme_init    (void *(*pgalloc)(int), void (*pgfree)(void *));
void     protect     (AddrSpace *as);
void     unprotect   (AddrSpace *as);
void     map         (AddrSpace *as, void *vaddr, void *paddr, int prot);
Context *ucontext    (AddrSpace *as, Area kstack, void *entry);

// ---------------------- MPE: Multi-Processing ----------------------
bool     mpe_init    (void (*entry)());
int      cpu_count   (void);
int      cpu_current (void);
int      atomic_xchg (int *addr, int newval);

#ifdef __cplusplus
}
#endif

#endif
