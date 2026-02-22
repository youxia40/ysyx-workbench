#include <am.h>//IOE分发表实现,把AM寄存器访问路由到对应设备处理函数
#include <klib-macros.h>

void __am_timer_init();//初始化定时器相关状态

void __am_timer_rtc(AM_TIMER_RTC_T *);//读取RTC日历信息
void __am_timer_uptime(AM_TIMER_UPTIME_T *);//读取系统运行时间
void __am_input_keybrd(AM_INPUT_KEYBRD_T *);//读取键盘事件
void __am_gpu_init();//初始化GPU
void __am_gpu_config(AM_GPU_CONFIG_T *);//读取GPU配置
void __am_gpu_status(AM_GPU_STATUS_T *);//读取GPU状态
void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *);//执行帧缓冲绘制

static void __am_timer_config(AM_TIMER_CONFIG_T *cfg) { cfg->present = true; cfg->has_rtc = true; }//声明NPC存在定时器且支持rtc寄存器
static void __am_input_config(AM_INPUT_CONFIG_T *cfg) { cfg->present = true;  }//声明NPC存在输入设备接口
static void __am_uart_config(AM_UART_CONFIG_T *cfg)   { cfg->present = false; }//与NEMU一致,IOE层不暴露UART设备

typedef void (*handler_t)(void *buf);//统一的设备处理函数签名
static void *lut[128] = {//IOE查找表:索引=AM_DEVREG编号,值=对应处理函数
  [AM_TIMER_CONFIG] = __am_timer_config,//定时器配置
  [AM_TIMER_RTC   ] = __am_timer_rtc,//实时时钟
  [AM_TIMER_UPTIME] = __am_timer_uptime,//定时器相关
  [AM_INPUT_CONFIG] = __am_input_config,//输入设备配置
  [AM_INPUT_KEYBRD] = __am_input_keybrd,//键盘输入
  [AM_GPU_CONFIG  ] = __am_gpu_config,//图形处理单元配置
  [AM_GPU_FBDRAW  ] = __am_gpu_fbdraw,//帧缓冲区绘制
  [AM_GPU_STATUS  ] = __am_gpu_status,//图形处理单元状态
  [AM_UART_CONFIG]  = __am_uart_config,//串口配置
};

static void fail(void *buf) { panic("access nonexist register"); }//未实现寄存器访问统一报错

bool ioe_init() {//初始化IO设备
  for (int i = 0; i < LENGTH(lut); i++) {
    if (!lut[i]) {
      lut[i] = fail;//把未绑定条目全部指向fail,防止野指针调用
    }
  }
  __am_gpu_init();//初始化GPU状态
  __am_timer_init();//初始化定时器基准
  return true;
}

void ioe_read (int reg, void *buf) { ((handler_t)lut[reg])(buf); }//统一读入口
void ioe_write(int reg, void *buf) { ((handler_t)lut[reg])(buf); }//统一写入口
