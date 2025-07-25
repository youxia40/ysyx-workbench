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

__EXPORT void difftest_memcpy(paddr_t addr, void *buf, size_t n, bool direction) {
  if (direction) {
        // 从REF内存复制到DUT
        // 对于NEMU作为REF，这个功能通常不需要实现
    } else {
        // 从DUT内存复制到REF
        // 复制NPC的内存状态到NEMU
        for (size_t i = 0; i < n; i++) {
            paddr_write(addr + i, *(uint8_t *)(buf + i), 8);
        }
    }
}

__EXPORT void difftest_regcpy(void *dut, bool direction) {
  CPU_state *ref = (CPU_state *)dut;
    
    if (direction) {
        // 从REF复制到DUT (direction == true)
        for (int i = 0; i < 32; i++) {
            ref->gpr[i] = cpu.gpr[i];
        }
        ref->pc = cpu.pc;
    } else {
        // 从DUT复制到REF (direction == false)
        for (int i = 0; i < 32; i++) {
            cpu.gpr[i] = ref->gpr[i];
        }
        cpu.pc = ref->pc;
    }
}

__EXPORT void difftest_exec(uint64_t n) {
  cpu_exec(n);
}

__EXPORT void difftest_raise_intr(word_t NO) {
  assert(0);
}



__EXPORT void difftest_init(int port) {
  void init_mem();
  init_mem();
  /* Perform ISA dependent initialization. */
  init_isa();
}
