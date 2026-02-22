#include <am.h>//定时器设备抽象实现,对外提供uptime与rtc接口
#include <npc.h>

void __am_timer_init() {
}

void __am_timer_uptime(AM_TIMER_UPTIME_T *uptime) {
  uint32_t low = inl(RTC_ADDR);//RTC低32位(微秒计数低位)
  uint32_t high = inl(RTC_ADDR + 4);//RTC高32位(读高位时设备会刷新锁存)
  uptime->us = ((uint64_t)high << 32) | low;//拼接成完整64位运行时间(us)
}

void __am_timer_rtc(AM_TIMER_RTC_T *rtc) {
  rtc->second = 0;//与NEMU实现保持一致:当前不提供完整日历
  rtc->minute = 0;
  rtc->hour   = 0;
  rtc->day    = 0;
  rtc->month  = 0;
  rtc->year   = 1900;
}
