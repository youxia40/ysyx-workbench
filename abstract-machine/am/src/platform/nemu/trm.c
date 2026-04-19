#include <am.h>
#include <nemu.h>

extern char _heap_start;
int main(const char *args);

Area heap = RANGE(&_heap_start, PMEM_END);//导出给klib的堆内存范围
static const char mainargs[MAINARGS_MAX_LEN] = MAINARGS_PLACEHOLDER; // defined in CFLAGS,nemu.mk定义的MAINARGS_PLACEHOLDER占位符会被Makefile中的insert-arg规则替换为mainargs

void putch(char ch) {    //输出字符到串口
  outb(SERIAL_PORT, ch);
}

void halt(int code) {
  nemu_trap(code);            //结束程序的运行,展开为内连汇编asm volatile("mv a0, %0; ebreak" : :"r"(code));

  // should not reach here
  while (1);
}

void _trm_init() {//设置main函数入口地址
  int ret = main(mainargs);//调用main(mainargs),拿到返回值ret.
  //注意：main(mainargs)是「要在RISCV目标程序里写的那个main函数（也就是被加载到0x80000000、在NPC里跑的程序入口）」，定义在第5、12行
  halt(ret);                  //调用halt(ret)终止并把返回码交给环境
}