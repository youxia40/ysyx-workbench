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
#include <generated/autoconf.h>
#include <memory/host.h>
#include <memory/paddr.h>
#include <device/mmio.h>
#include <isa.h>
#include <common.h>

#if   defined(CONFIG_PMEM_MALLOC)
static uint8_t *pmem = NULL;
#else // CONFIG_PMEM_GARRAY
static uint8_t pmem[CONFIG_MSIZE] PG_ALIGN = {};            //模拟内存(物理)
#endif

uint8_t* guest_to_host(paddr_t paddr) { return pmem + paddr - CONFIG_MBASE; }
paddr_t host_to_guest(uint8_t *haddr) { return haddr - pmem + CONFIG_MBASE; }

static word_t pmem_read(paddr_t addr, int len) {
  word_t ret = host_read(guest_to_host(addr), len);
  return ret;
}

static void pmem_write(paddr_t addr, int len, word_t data) {
  host_write(guest_to_host(addr), len, data);
}

static void out_of_bound(paddr_t addr) {                      //处理物理内存访问越界的情况
  panic("address = " FMT_PADDR " is out of bound of pmem [" FMT_PADDR ", " FMT_PADDR "] at pc = " FMT_WORD,
      addr, PMEM_LEFT, PMEM_RIGHT, cpu.pc);
}

void init_mem() {
#if   defined(CONFIG_PMEM_MALLOC)
  pmem = malloc(CONFIG_MSIZE);
  assert(pmem);
#endif
  IFDEF(CONFIG_MEM_RANDOM, memset(pmem, rand(), CONFIG_MSIZE));
  Log("physical memory area [" FMT_PADDR ", " FMT_PADDR "]", PMEM_LEFT, PMEM_RIGHT);
}

extern void mtrace_read(paddr_t addr, int len);                      //读取物理内存时的跟踪函数
extern void mtrace_write(paddr_t addr, int len, word_t data);       //写入物理内存时的跟踪函数

word_t paddr_read(paddr_t addr, int len) {                             //读取物理内存
  if (likely(in_pmem(addr))) {                                   //如果地址在物理内存范围内，则调用pmem_read函数读取物理内存
#ifdef CONFIG_MTRACE
    mtrace_read(addr, len);
#endif
    return pmem_read(addr, len);
  }

  IFDEF(CONFIG_DEVICE, return mmio_read(addr, len));                    //如果是设备内存映射，则调用mmio_read函数读取设备内存
  out_of_bound(addr);                                                //如果地址不在物理内存范围内，则触发错误
  return 0;
}

void paddr_write(paddr_t addr, int len, word_t data) {              //写入物理内存
  if (likely(in_pmem(addr))) {                                  //如果地址在物理内存范围内，则调用pmem_write函数写入物理内存
#ifdef CONFIG_MTRACE
    mtrace_write(addr, len, data);
#endif
    pmem_write(addr, len, data);
    return;
  }

  IFDEF(CONFIG_DEVICE, mmio_write(addr, len, data); return);
  out_of_bound(addr);
}
