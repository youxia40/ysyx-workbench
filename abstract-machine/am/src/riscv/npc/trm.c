#include <am.h>//TRM平台入口实现,负责把AM程序接到NPC设备模型
#include <klib-macros.h>
#include <npc.h>

/*
最简单的计算机称为"图灵机":
  结构上, TRM有存储器, 有PC, 有寄存器, 有加法器;
  工作方式上, TRM不断地重复以下过程: 从PC指示的存储器位置取出指令, 执行指令, 然后更新PC.
*/


extern char _heap_start;//链接脚本提供的堆起点符号
int main(const char *args);//用户程序入口(AM约定签名)

extern char _pmem_start;//链接脚本提供的程序装载基址符号
#define PMEM_SIZE (128 * 1024 * 1024)
#define PMEM_END  ((uintptr_t)&_pmem_start + PMEM_SIZE)

Area heap = RANGE(&_heap_start, PMEM_END);  //导出给klib的堆内存范围
static const char mainargs[MAINARGS_MAX_LEN] = MAINARGS_PLACEHOLDER; //编译期写入占位串,运行前由insert-arg替换
//AM把“命令行参数”塞成一个字符串mainargs

void putch(char ch) {
  outb(SERIAL_PORT, (uint8_t)ch);//所有字符输出最终写到MMIO串口寄存器
}
//putch是AM/klib里常见的输出接口（例如printf最终可能会调用putch输出字符）

void halt(int code) {
  //传递退出码给仿真环境
  asm volatile("mv a0, %0" : : "r"(code));
  
  //通过内联汇编插入ebreak指令
  asm volatile("ebreak");
  
  while (1) {
  }
}

void _trm_init() {          //AM约定的“启动入口”(通常由start.S调用它)
  int ret = main(mainargs);   //把mainargs传给用户main,保持与NEMU/AM接口一致
  //注意:main(mainargs)是要在RISCV目标程序里写的main函数(也就是被加载到0x80000000、在NPC里跑的程序入口),定义在第5、12行
  halt(ret);                  //调用halt(ret)终止并把返回码交给环境
}
