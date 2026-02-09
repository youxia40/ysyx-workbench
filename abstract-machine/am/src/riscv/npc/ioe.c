#include <am.h>
#include <klib-macros.h>

void __am_timer_init();

void __am_timer_rtc(AM_TIMER_RTC_T *);
void __am_timer_uptime(AM_TIMER_UPTIME_T *);
void __am_input_keybrd(AM_INPUT_KEYBRD_T *);

static void __am_timer_config(AM_TIMER_CONFIG_T *cfg) { cfg->present = true; cfg->has_rtc = true; }
static void __am_input_config(AM_INPUT_CONFIG_T *cfg) { cfg->present = true;  }
static void __am_uart_config(AM_INPUT_CONFIG_T *cfg) { cfg->present = false;  }

typedef void (*handler_t)(void *buf);
static void *lut[128] = {//查找表
  [AM_TIMER_CONFIG] = __am_timer_config,//定时器配置
  [AM_TIMER_RTC   ] = __am_timer_rtc,//实时时钟
  [AM_TIMER_UPTIME] = __am_timer_uptime,//定时器相关
  [AM_INPUT_CONFIG] = __am_input_config,//输入设备配置
  [AM_INPUT_KEYBRD] = __am_input_keybrd,//键盘输入
  [AM_UART_CONFIG]  = __am_uart_config,//串口配置
};

static void fail(void *buf) { panic("access nonexist register"); }

bool ioe_init() {//初始化IO设备
  for (int i = 0; i < LENGTH(lut); i++)
    if (!lut[i]) lut[i] = fail;
  __am_timer_init();//初始化定时器
  return true;
}

void ioe_read (int reg, void *buf) { ((handler_t)lut[reg])(buf); }
void ioe_write(int reg, void *buf) { ((handler_t)lut[reg])(buf); }
