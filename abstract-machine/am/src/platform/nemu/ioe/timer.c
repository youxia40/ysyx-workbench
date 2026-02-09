#include <am.h>
#include <nemu.h>

void __am_timer_init() {
}

void __am_timer_uptime(AM_TIMER_UPTIME_T *uptime) {//获取系统运行时间
  //注：inl来自src/riscv32/riscv32.h,RTC_ADDR来自src/platform/nemu/include/nemu.h
  uint32_t low = inl(RTC_ADDR);
  uint32_t high = inl(RTC_ADDR + 4);
  uint64_t t = ((uint64_t)high << 32) | low;

  uptime->us = t;
}

void __am_timer_rtc(AM_TIMER_RTC_T *rtc) {//获取实时时钟
  rtc->second = 0;
  rtc->minute = 0;
  rtc->hour   = 0;
  rtc->day    = 0;
  rtc->month  = 0;
  rtc->year   = 1900;
}
