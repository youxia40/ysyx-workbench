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

#include <device/map.h>
#include <device/alarm.h>
#include <utils.h>

static uint32_t *rtc_port_base = NULL;

static void rtc_io_handler(uint32_t offset, int len, bool is_write) {//实时时钟IO处理函数
  assert(offset == 0 || offset == 4);
  if (!is_write && offset == 4) {//CPU访问RTC高32位时，填充当前时间的高低32位到设备寄存器
    uint64_t us = get_time();//获取当前时间，单位为微秒
    rtc_port_base[0] = (uint32_t)us;
    rtc_port_base[1] = us >> 32;
  }
}

#ifndef CONFIG_TARGET_AM
static void timer_intr() {//定时器中断处理函数
  if (nemu_state.state == NEMU_RUNNING) {//仅在NEMU_RUNNING状态下触发中断
    extern void dev_raise_intr();//声明触发中断函数
    dev_raise_intr();//触发中断
  }
}
#endif

void init_timer() {//初始化定时器设备，映射到地址空间，并注册中断处理函数
  rtc_port_base = (uint32_t *)new_space(8);
#ifdef CONFIG_HAS_PORT_IO
  add_pio_map ("rtc", CONFIG_RTC_PORT, rtc_port_base, 8, rtc_io_handler);
#else
  add_mmio_map("rtc", CONFIG_RTC_MMIO, rtc_port_base, 8, rtc_io_handler);
  //把RTC设备挂到MMIO地址区间（映射实时时钟设备,CPU访存命中这段地址时，进入rtc_io_handler，用get_time()填充高低32位并返回给CPU）
#endif
  IFNDEF(CONFIG_TARGET_AM, add_alarm_handle(timer_intr));//注册定时器中断处理函数
}
