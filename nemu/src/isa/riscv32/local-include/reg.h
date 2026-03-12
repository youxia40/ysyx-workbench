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

#ifndef __RISCV_REG_H__
#define __RISCV_REG_H__

#include <common.h>

static inline int check_reg_idx(int idx) {
  IFDEF(CONFIG_RT_CHECK, assert(idx >= 0 && idx < MUXDEF(CONFIG_RVE, 16, 32)));
  return idx;
}

#define gpr(idx) (cpu.gpr[check_reg_idx(idx)])

enum {//CSR寄存器编号（都属于M级，地址高两位为11）
  CSR_MSTATUS = 0x300,
  CSR_MTVEC = 0x305,//机器模式下存储中断处理程序的基地址和模式
  CSR_MEPC = 0x341,
  CSR_MCAUSE  = 0x342,
  CSR_MVENDORID = 0xF11,
  CSR_MARCHID = 0xF12,
};

extern word_t CSRs[4096];//CSR编号是12位，范围正好0~4095，用数组来存储所有CSR寄存器的值，CSRs[CSR_MSTATUS]就能访问mstatus寄存器的值

static inline const char* reg_name(int idx) {
  extern const char* regs[];
  return regs[check_reg_idx(idx)];
}

#endif
