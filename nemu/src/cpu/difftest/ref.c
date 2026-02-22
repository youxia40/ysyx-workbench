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
#include <cpu/cpu.h>
#include <difftest-def.h>
#include <memory/paddr.h>
#include <string.h>

// 在 DUT host 内存 buf 和 REF guest 内存 addr 之间拷贝
// direction == DIFFTEST_TO_DUT:  REF -> DUT  (guest_to_host(addr) -> buf)
// direction == DIFFTEST_TO_REF:  DUT -> REF  (buf -> guest_to_host(addr))
__EXPORT void difftest_memcpy(paddr_t addr, void *buf, size_t n, bool direction) {//内存同步接口，约定中地址是REF的物理地址，buf是DUT的host地址
  if (direction == DIFFTEST_TO_DUT) {
    memcpy(buf, guest_to_host(addr), n);
  } else if (direction == DIFFTEST_TO_REF) {
    memcpy(guest_to_host(addr), buf, n);
  }
}

// 在 NEMU 的 cpu 和外部 DUT 状态之间拷贝寄存器+PC
// direction == DIFFTEST_TO_DUT:  cpu -> dut
// direction == DIFFTEST_TO_REF:  dut -> cpu
__EXPORT void difftest_regcpy(void *dut, bool direction) {
  if (direction == DIFFTEST_TO_DUT) {
    memcpy(dut, &cpu, DIFFTEST_REG_SIZE);
  } else if (direction == DIFFTEST_TO_REF) {
    memcpy(&cpu, dut, DIFFTEST_REG_SIZE);
  }
}

// 让 NEMU 执行 n 条指令
__EXPORT void difftest_exec(uint64_t n) {
  cpu_exec(n);
}

// 抛出一个中断/异常号 NO（当前 NPC 阶段基本不用）
__EXPORT void difftest_raise_intr(uint64_t NO) {
#if defined(CONFIG_ISA_riscv)
  cpu.pc = isa_raise_intr((word_t)NO, cpu.pc);
#else
  (void)NO;
#endif
}

// 初始化 REF：初始化内存和 ISA 子系统
__EXPORT void difftest_init(int port) {
  (void)port;
  void init_mem(void);
  init_mem();
  init_isa();
}

