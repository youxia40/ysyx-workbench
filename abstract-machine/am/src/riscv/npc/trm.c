#include <am.h>
#include <klib-macros.h>

/*
最简单的计算机称为"图灵机":
  结构上, TRM有存储器, 有PC, 有寄存器, 有加法器;
  工作方式上, TRM不断地重复以下过程: 从PC指示的存储器位置取出指令, 执行指令, 然后更新PC.
*/


extern char _heap_start;
int main(const char *args);

extern char _pmem_start;
#define PMEM_SIZE (128 * 1024 * 1024)
#define PMEM_END  ((uintptr_t)&_pmem_start + PMEM_SIZE)

Area heap = RANGE(&_heap_start, PMEM_END);  //用于指示堆区的起始和末尾
static const char mainargs[MAINARGS_MAX_LEN] = MAINARGS_PLACEHOLDER; // defined in CFLAGS
//AM把“命令行参数”塞成一个字符串mainargs

void putch(char ch) {
}
//putch是AM/klib里常见的输出接口（例如printf最终可能会调用putch输出字符）

void halt(int code) {
  //传递退出码给仿真环境
  asm volatile("mv a0, %0" : : "r"(code));
  
  //通过内联汇编插入 ebreak 指令
  asm volatile("ebreak");
  
  while (1);
}

void _trm_init() {          //AM约定的“启动入口”（通常由 start.S 调用它）
  int ret = main(mainargs);   //调用main(mainargs),拿到返回值ret.
  //注意：main(mainargs)是「要在RISCV目标程序里写的那个main函数（也就是被加载到 0x80000000、在NPC里跑的程序入口）」，定义在第5、12行
  halt(ret);                  //调用halt(ret)终止并把返回码交给环境
}
