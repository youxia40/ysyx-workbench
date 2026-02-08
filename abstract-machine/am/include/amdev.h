#ifndef __AMDEV_H__
#define __AMDEV_H__

// **MAY SUBJECT TO CHANGE IN THE FUTURE**

#define AM_DEVREG(id, reg, perm, ...) \
  enum { AM_##reg = (id) }; \
  typedef struct { __VA_ARGS__; } AM_##reg##_T;


//抽象机器设备寄存器定义, 包括寄存器ID, 读写权限, 以及寄存器内容、格式等信息
AM_DEVREG( 1, UART_CONFIG,  RD, bool present);//串口配置
                                //是否存在串口设备
AM_DEVREG( 2, UART_TX,      WR, char data);//串口发送数据
                                //要发送的1字节字符
AM_DEVREG( 3, UART_RX,      RD, char data);//串口接收数据
                                //从串口接受到的1字节字符
AM_DEVREG( 4, TIMER_CONFIG, RD, bool present, has_rtc);//定时器配置
                                //读年月日秒    //运行时间
AM_DEVREG( 5, TIMER_RTC,    RD, int year, month, day, hour, minute, second);//实时时钟
                                //完整日历
AM_DEVREG( 6, TIMER_UPTIME, RD, uint64_t us);//定时器相关
                                //系统启动以来的时长
AM_DEVREG( 7, INPUT_CONFIG, RD, bool present);//输入设备配置
                                //是否有输入设备作用
AM_DEVREG( 8, INPUT_KEYBRD, RD, bool keydown; int keycode);//键盘输入
                                //是否按下按键  //扫描码
AM_DEVREG( 9, GPU_CONFIG,   RD, bool present, has_accel; int width, height, vmemsz);                 //AM显示控制器信息, 可读出屏幕大小信息width和height，
                                //是否存在GPU  //支持硬件加速//宽      //高    //显存可用内存大小           //另外AM假设系统在运行过程中, 屏幕大小不会发生变化.
AM_DEVREG(10, GPU_STATUS,   RD, bool ready);//图形处理单元状态
                                //GPU是否就绪
AM_DEVREG(11, GPU_FBDRAW,   WR, int x, y; void *pixels; int w, h; bool sync);                        //写入绘图信息向屏幕(x, y)坐标处绘制w*h的矩形图像,
                                //左上角坐标//像素数据指针  //画的宽高 //立即刷新到屏幕                     //图像像素按行优先方式存储在pixels中, 每个像素用32位整数
                                                                                                     //以00RRGGBB的方式描述颜色. 若sync为true, 则马上将帧缓冲中的内容同步到屏幕上.
AM_DEVREG(12, GPU_MEMCPY,   WR, uint32_t dest; void *src; int size);//图形处理单元内存拷贝
                                //GPU侧目标地址偏移，显存地址
AM_DEVREG(13, GPU_RENDER,   WR, uint32_t root);//图形处理单元渲染
                                //出发一次渲染
AM_DEVREG(14, AUDIO_CONFIG, RD, bool present; int bufsize);//音频配置
                                //有没有音频设备//流缓冲区大小
AM_DEVREG(15, AUDIO_CTRL,   WR, int freq, channels, samples);//音频控制
                                //采样率   //声道数   //播放的周期长度
AM_DEVREG(16, AUDIO_STATUS, RD, int count);//音频状态
                                //流缓冲区内已写入的字节长度
AM_DEVREG(17, AUDIO_PLAY,   WR, Area buf);//音频播放
                                //要播放的音频数据去见
AM_DEVREG(18, DISK_CONFIG,  RD, bool present; int blksz, blkcnt);//磁盘配置
AM_DEVREG(19, DISK_STATUS,  RD, bool ready);//磁盘状态
AM_DEVREG(20, DISK_BLKIO,   WR, bool write; void *buf; int blkno, blkcnt);//磁盘块输入输出
AM_DEVREG(21, NET_CONFIG,   RD, bool present);//网络配置
AM_DEVREG(22, NET_STATUS,   RD, int rx_len, tx_len);//网络状态
AM_DEVREG(23, NET_TX,       WR, Area buf);//网络发送
AM_DEVREG(24, NET_RX,       WR, Area buf);//网络接收

// Input

#define AM_KEYS(_) \
  _(ESCAPE) _(F1) _(F2) _(F3) _(F4) _(F5) _(F6) _(F7) _(F8) _(F9) _(F10) _(F11) _(F12) \
  _(GRAVE) _(1) _(2) _(3) _(4) _(5) _(6) _(7) _(8) _(9) _(0) _(MINUS) _(EQUALS) _(BACKSPACE) \
  _(TAB) _(Q) _(W) _(E) _(R) _(T) _(Y) _(U) _(I) _(O) _(P) _(LEFTBRACKET) _(RIGHTBRACKET) _(BACKSLASH) \
  _(CAPSLOCK) _(A) _(S) _(D) _(F) _(G) _(H) _(J) _(K) _(L) _(SEMICOLON) _(APOSTROPHE) _(RETURN) \
  _(LSHIFT) _(Z) _(X) _(C) _(V) _(B) _(N) _(M) _(COMMA) _(PERIOD) _(SLASH) _(RSHIFT) \
  _(LCTRL) _(APPLICATION) _(LALT) _(SPACE) _(RALT) _(RCTRL) \
  _(UP) _(DOWN) _(LEFT) _(RIGHT) _(INSERT) _(DELETE) _(HOME) _(END) _(PAGEUP) _(PAGEDOWN)

#define AM_KEY_NAMES(key) AM_KEY_##key,
enum {
  AM_KEY_NONE = 0,
  AM_KEYS(AM_KEY_NAMES)
};

// GPU

#define AM_GPU_TEXTURE  1
#define AM_GPU_SUBTREE  2
#define AM_GPU_NULL     0xffffffff

typedef uint32_t gpuptr_t;

struct gpu_texturedesc {
  uint16_t w, h;
  gpuptr_t pixels;
} __attribute__((packed));

struct gpu_canvas {
  uint16_t type, w, h, x1, y1, w1, h1;
  gpuptr_t sibling;
  union {
    gpuptr_t child;
    struct gpu_texturedesc texture;
  };
} __attribute__((packed));

#endif
