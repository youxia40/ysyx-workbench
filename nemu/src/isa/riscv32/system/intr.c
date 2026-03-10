/***************************************************************************************
* Copyright (c) 2014-2024 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include <isa.h>
#include "../local-include/reg.h"

extern void etrace_trap(word_t no, vaddr_t epc, vaddr_t handler);

word_t isa_raise_intr(word_t NO, vaddr_t epc) {//触发一个中断/异常，NO是中断/异常号，epc是发生中断/异常时的程序计数器值
  //负责处理发生的中断/异常事件，保存相关状态，并返回中断/异常处理程序的入口地址

  /* TODO: Trigger an interrupt/exception with ``NO''.使用'NO'触发中断/异常.
   * Then return the address of the interrupt/exception vector.然后返回中断/异常向量的地址.
   */

  //通用寄存器镜像不在这里做，而在trap.S的PUSH/POP完成
  CSRs[CSR_MEPC] = epc;//把发生异常的PC保存到mepc寄存器中
  CSRs[CSR_MCAUSE] = NO;//把触发异常的原因保存到mcause寄存器中

  word_t mtvec = CSRs[CSR_MTVEC];//从mtvec寄存器中读取中断/异常向量基地址和模式，mtvec的最低两位表示模式，剩余位表示基地址
  word_t handler;
  if ((mtvec & 0x3) == 1) {//mtvec的最低两位是11，则表示vectored模式，向量地址为基地址加上4倍的中断/异常号
    /*
    RISC-V规定：
    同步异常：仍然跳到Pc = BASE
    异步中断：跳到PC = BASE + 4 × mcause里cause编号
    */
    handler = (mtvec & ~0x3u) + 4 * NO;
  }
  else {
    handler = (mtvec & ~0x3u);
  }

  etrace_trap(NO, epc, handler);//调用etrace_trap函数进行事件追踪，记录发生的中断/异常信息，是陷入异常时从“出事指令地址”跳到“异常入口地址”
  return handler;
}

word_t isa_query_intr() {//查询是否有待处理的中断，返回中断号，如果没有则返回INTR_EMPTY

  return INTR_EMPTY;
}
