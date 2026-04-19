#include "common.h"
#include "device.h"
#include "map.h"
#include "mmio.h"

#include <assert.h>
#include <stdint.h>
#include <sys/time.h>

static uint32_t *rtc_port_base = NULL;

static uint64_t get_time_us(void) {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return (uint64_t)tv.tv_sec * 1000000ull + (uint64_t)tv.tv_usec;
}

static void rtc_io_handler(uint32_t offset, int len, bool is_write) {
  //实时时钟IO处理函数
  assert(offset == 0 || offset == 4);
  if (!is_write && offset == 4) {
    //CPU访问RTC高32位时，填充当前时间的高低32位到设备寄存器
    uint64_t us = get_time_us();
    rtc_port_base[0] = (uint32_t)us;
    rtc_port_base[1] = (uint32_t)(us >> 32);
  }
}

void init_timer(void) {
  //初始化定时器设备，映射到地址空间，并注册中断处理函数
  rtc_port_base = (uint32_t *)new_space(8); //分配8字节的空间存储当前时间的低32位和高32位
  add_mmio_map("rtc", MMIO_RTC_LO_ADDR, rtc_port_base, 8, rtc_io_handler);
}
