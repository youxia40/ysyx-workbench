#include "mmio.h"
#include "common.h"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <termios.h>
#include <unistd.h>

#if NPC_USE_SDL
#include <SDL2/SDL.h>
#endif

#define GPU_WIDTH  400u
#define GPU_HEIGHT 300u
#define GPU_VMEM_SIZE (GPU_WIDTH * GPU_HEIGHT * 4u)

#define KEYDOWN_MASK 0x8000u
#define INPUT_Q_LEN 1024

#define NPC_KEYS(f) \
  f(ESCAPE) f(F1) f(F2) f(F3) f(F4) f(F5) f(F6) f(F7) f(F8) f(F9) f(F10) f(F11) f(F12) \
  f(GRAVE) f(1) f(2) f(3) f(4) f(5) f(6) f(7) f(8) f(9) f(0) f(MINUS) f(EQUALS) f(BACKSPACE) \
  f(TAB) f(Q) f(W) f(E) f(R) f(T) f(Y) f(U) f(I) f(O) f(P) f(LEFTBRACKET) f(RIGHTBRACKET) f(BACKSLASH) \
  f(CAPSLOCK) f(A) f(S) f(D) f(F) f(G) f(H) f(J) f(K) f(L) f(SEMICOLON) f(APOSTROPHE) f(RETURN) \
  f(LSHIFT) f(Z) f(X) f(C) f(V) f(B) f(N) f(M) f(COMMA) f(PERIOD) f(SLASH) f(RSHIFT) \
  f(LCTRL) f(APPLICATION) f(LALT) f(SPACE) f(RALT) f(RCTRL) \
  f(UP) f(DOWN) f(LEFT) f(RIGHT) f(INSERT) f(DELETE) f(HOME) f(END) f(PAGEUP) f(PAGEDOWN)

#define NPC_KEY_NAME(k) NPC_KEY_##k,
enum {
  NPC_KEY_NONE = 0,
  NPC_KEYS(NPC_KEY_NAME)
};

static char serial_buf[4096];
static int serial_buf_len = 0;

static int io_inited = 0;//输入系统是否初始化

static uint32_t kbd_q[INPUT_Q_LEN];//键盘事件队列(bit15=keydown,低位=AM键码)
static int kbd_q_head = 0;//键盘队列读指针
static int kbd_q_tail = 0;//键盘队列写指针

static uint8_t uart_rx_q[INPUT_Q_LEN];//串口接收字节队列
static int uart_rx_head = 0;//串口队列读指针
static int uart_rx_tail = 0;//串口队列写指针

static uint8_t gpu_vmem[GPU_VMEM_SIZE];//软件帧缓冲
static uint32_t gpu_sync = 0;//SYNC寄存器镜像
static int gpu_sync_seen = 0;//是否收到过首个SYNC
static int gpu_dirty = 0;//帧缓冲是否有改动

static uint64_t rtc_latched_us = 0;//读LO时锁存值
static uint64_t rtc_boot_us = 0;//启动时间基准

static struct termios stdin_term_orig;//stdin原始终端属性
static int stdin_term_saved = 0;//是否保存过原始终端属性

//SDL窗口和键码映射状态
#if NPC_USE_SDL
static SDL_Window *gpu_window = NULL;
static SDL_Renderer *gpu_renderer = NULL;
static SDL_Texture *gpu_texture = NULL;
static int gpu_sdl_inited = 0;

static uint32_t keymap[512] = {0};//SDL扫描码->AM键码映射表
static int keymap_inited = 0;//键码映射是否已初始化
#define SDL_KEYMAP(k) keymap[SDL_SCANCODE_##k] = NPC_KEY_##k;
#endif

//输入环形队列
static inline int q_next(int idx) {
//索引推进到下一个槽位
  return (idx + 1) % INPUT_Q_LEN;
}

static inline void kbd_enqueue(uint32_t ev) {
//队列满时覆盖最老事件,这样最新按键不会丢
  int next = q_next(kbd_q_tail);
  if (next == kbd_q_head) {
    kbd_q_head = q_next(kbd_q_head);
  }
  kbd_q[kbd_q_tail] = ev;
  kbd_q_tail = next;
}

static inline uint32_t kbd_dequeue(void) {
//无键盘事件时返回NPC_KEY_NONE
  uint32_t key = NPC_KEY_NONE;
  if (kbd_q_head != kbd_q_tail) {
    key = kbd_q[kbd_q_head];
    kbd_q_head = q_next(kbd_q_head);
  }
  return key;
}

static inline void uart_enqueue(uint8_t ch) {
//串口接收队列也用同样策略
  int next = q_next(uart_rx_tail);
  if (next == uart_rx_head) {
    uart_rx_head = q_next(uart_rx_head);
  }
  uart_rx_q[uart_rx_tail] = ch;
  uart_rx_tail = next;
}

static inline uint32_t uart_dequeue(void) {
//无串口输入时返回0xff
  uint32_t ch = 0xffu;
  if (uart_rx_head != uart_rx_tail) {
    ch = (uint32_t)uart_rx_q[uart_rx_head];
    uart_rx_head = q_next(uart_rx_head);
  }
  return ch;
}

static inline uint32_t ascii_to_am_key(unsigned char ch) {//把ASCII字符映射到AM键码,仅支持常用可见字符，特殊键由SDL事件直接注入
  if (ch >= 'a' && ch <= 'z') return NPC_KEY_A + (uint32_t)(ch - 'a');
  if (ch >= 'A' && ch <= 'Z') return NPC_KEY_A + (uint32_t)(ch - 'A');
  if (ch >= '1' && ch <= '9') return NPC_KEY_1 + (uint32_t)(ch - '1');
  if (ch == '0') return NPC_KEY_0;

  switch (ch) {
    case ' ': return NPC_KEY_SPACE;
    case '\n':
    case '\r': return NPC_KEY_RETURN;
    case '\t': return NPC_KEY_TAB;
    case 127://ASCII DEL键通常被终端映射为Backspace
    case '\b': return NPC_KEY_BACKSPACE;
    case '-': return NPC_KEY_MINUS;
    case '=': return NPC_KEY_EQUALS;
    case '[': return NPC_KEY_LEFTBRACKET;
    case ']': return NPC_KEY_RIGHTBRACKET;
    case '\\': return NPC_KEY_BACKSLASH;
    case ';': return NPC_KEY_SEMICOLON;
    case '\'': return NPC_KEY_APOSTROPHE;
    case ',': return NPC_KEY_COMMA;
    case '.': return NPC_KEY_PERIOD;
    case '/': return NPC_KEY_SLASH;
    case '`': return NPC_KEY_GRAVE;
    default: return NPC_KEY_NONE;
  }
}

static inline void serial_flush(void) {//把串口输出缓存刷到stdout
  if (serial_buf_len <= 0) {
    return;
  }
  fwrite(serial_buf, 1, (size_t)serial_buf_len, stdout);
  fflush(stdout);//实时输出
  serial_buf_len = 0;
}

static inline void serial_putc(char ch) {
#if NPC_ENABLE_ASSERT
  assert(serial_buf_len >= 0 && serial_buf_len < (int)sizeof(serial_buf));
#endif
  serial_buf[serial_buf_len++] = ch;
//按字符立即刷出,方便实时看到日志
  serial_flush();
}

static void stdin_restore_term(void) {
  if (stdin_term_saved) {
    (void)tcsetattr(STDIN_FILENO, TCSANOW, &stdin_term_orig);
  }
}

//stdin采集并写入UART输入队列
static void io_init_once(void) {//把stdin设成非阻塞,避免设备读取把CPU卡住
  if (io_inited) {
    return;
  }
  io_inited = 1;

  {
//先把stdin设成非阻塞,避免设备读取把CPU卡住
    int flags = fcntl(STDIN_FILENO, F_GETFL);
    if (flags >= 0) {
      (void)fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
    }
  }

  if (isatty(STDIN_FILENO)) {
    struct termios t;
    if (tcgetattr(STDIN_FILENO, &stdin_term_orig) == 0) {
      stdin_term_saved = 1;
      t = stdin_term_orig;
      t.c_lflag &= (tcflag_t)~(ICANON | ECHO);
      t.c_cc[VMIN] = 0;
      t.c_cc[VTIME] = 0;
      (void)tcsetattr(STDIN_FILENO, TCSANOW, &t);
      (void)atexit(stdin_restore_term);
    }
  }
}

static inline void pump_stdin_uart(void) {
  int i;
//每轮最多取32字节,避免I/O长期占用主循环
  for (i = 0; i < 32; i++) {
    unsigned char ch = 0;
    ssize_t n = read(STDIN_FILENO, &ch, 1);
    if (n == 1) {
//终端字符直接作为UART字节输入
      uart_enqueue(ch);
      {
        uint32_t key = ascii_to_am_key(ch);
        if (key != NPC_KEY_NONE) {
//终端输入仅注入keydown,对齐“只看DOWN”的测试诉求
          kbd_enqueue(KEYDOWN_MASK | key);
        }
      }
      continue;
    }
    if (n == 0) {
      break;
    }
    if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR) {
      break;
    }
    break;
  }
}

#if NPC_USE_SDL
//SDL事件轮询并分发到键盘/串口
static void gpu_window_init(void) {
  if (gpu_sdl_inited) {
    return;
  }
  gpu_sdl_inited = 1;

  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    fprintf(stderr, "[NPC][GPU] SDL_Init failed: %s\n", SDL_GetError());
    return;
  }

  if (SDL_CreateWindowAndRenderer((int)GPU_WIDTH * 2, (int)GPU_HEIGHT * 2,
      0, &gpu_window, &gpu_renderer) != 0) {
    fprintf(stderr, "[NPC][GPU] SDL_CreateWindowAndRenderer failed: %s\n", SDL_GetError());
    return;
  }

  SDL_SetWindowTitle(gpu_window, "NPC");
  gpu_texture = SDL_CreateTexture(gpu_renderer, SDL_PIXELFORMAT_ARGB8888,
      SDL_TEXTUREACCESS_STREAMING, (int)GPU_WIDTH, (int)GPU_HEIGHT);
  if (!gpu_texture) {
    fprintf(stderr, "[NPC][GPU] SDL_CreateTexture failed: %s\n", SDL_GetError());
    return;
  }

  fprintf(stderr, "[NPC][GPU] SDL window ready: %ux%u\n", GPU_WIDTH, GPU_HEIGHT);
}

static void gpu_window_refresh(void) {
//把软件帧缓冲推到SDL窗口
  gpu_window_init();
  if (!gpu_renderer || !gpu_texture) {
    return;
  }
  SDL_UpdateTexture(gpu_texture, NULL, gpu_vmem, (int)GPU_WIDTH * 4);
  SDL_RenderClear(gpu_renderer);
  SDL_RenderCopy(gpu_renderer, gpu_texture, NULL, NULL);
  SDL_RenderPresent(gpu_renderer);
}

static void init_keymap_once(void) {
  if (keymap_inited) {
    return;
  }
  keymap_inited = 1;
//把SDL扫描码映射到AM键码
  NPC_KEYS(SDL_KEYMAP)
}

static inline void pump_sdl_events(void) {
  SDL_Event ev;
  gpu_window_init();
  init_keymap_once();

  while (SDL_PollEvent(&ev)) {
    if (ev.type == SDL_QUIT) {
//把窗口关闭转换成ESC输入,便于上层程序处理退出
      kbd_enqueue(KEYDOWN_MASK | NPC_KEY_ESCAPE);
      continue;
    }

    if (ev.type != SDL_KEYDOWN && ev.type != SDL_KEYUP) {
      continue;
    }
    if (ev.key.repeat) {
      continue;
    }

    {
      SDL_Scancode sc = ev.key.keysym.scancode;
      uint32_t am_key = (sc < (SDL_Scancode)(sizeof(keymap) / sizeof(keymap[0]))) ? keymap[sc] : NPC_KEY_NONE;
      if (am_key == NPC_KEY_NONE) {
//未映射按键直接忽略,避免污染键盘设备语义
        continue;
      }

      if (ev.type == SDL_KEYDOWN) {
//在键盘设备里上报keydown事件
        kbd_enqueue(KEYDOWN_MASK | am_key);
      } else {
//在键盘设备里上报keyup事件
        kbd_enqueue(am_key);
      }
    }
  }
}
#else
static void gpu_window_refresh(void) {
}

static inline void pump_sdl_events(void) {
}
#endif

//输入统一入口
static inline void input_pump_all(void) {
//先收SDL事件,再收stdin字符
  io_init_once();
  pump_sdl_events();
  pump_stdin_uart();
}

static inline uint32_t keyboard_poll_event(void) {
//每次读键盘寄存器前先泵输入
  input_pump_all();
  return kbd_dequeue();
}

static inline uint32_t serial_poll_char(void) {
//仅在UART队列为空时再泵输入,避免持续串口轮询饿死键盘事件输出
  if (uart_rx_head == uart_rx_tail) {
    input_pump_all();
  }
  return uart_dequeue();
}

//RTC实现
static uint64_t get_time_us(void) {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return (uint64_t)tv.tv_sec * 1000000ull + (uint64_t)tv.tv_usec;
}

static uint64_t get_uptime_us(void) {
//第一次读取时记录启动时间基准
  uint64_t now = get_time_us();
  if (rtc_boot_us == 0) {
    rtc_boot_us = now;
  }
  return now - rtc_boot_us;
}

//帧缓冲读写
static inline int fb_in_range(uint32_t addr_aligned) {
//统一按word对齐地址判断是否命中FB区域
  return addr_aligned >= MMIO_FB_ADDR && addr_aligned < (MMIO_FB_ADDR + GPU_VMEM_SIZE);
}

static inline uint32_t fb_read_word(uint32_t addr_aligned) {
#if NPC_ENABLE_ASSERT
  assert((addr_aligned & 0x3u) == 0);
#endif
  if (!fb_in_range(addr_aligned)) {
    return 0;
  }

  {
    uint32_t off = addr_aligned - MMIO_FB_ADDR;
    if (off + 4 > GPU_VMEM_SIZE) {
      return 0;
    }

//按小端把4字节拼成一个32位字返回
    return ((uint32_t)gpu_vmem[off + 0] << 0) |
           ((uint32_t)gpu_vmem[off + 1] << 8) |
           ((uint32_t)gpu_vmem[off + 2] << 16) |
           ((uint32_t)gpu_vmem[off + 3] << 24);
  }
}

static inline void fb_write_word(uint32_t addr_aligned, uint32_t data, uint8_t mask) {
#if NPC_ENABLE_ASSERT
  assert((addr_aligned & 0x3u) == 0);
  assert((mask & 0xf0u) == 0);
#endif
  if (!fb_in_range(addr_aligned)) {
    return;
  }

  {
    uint32_t off = addr_aligned - MMIO_FB_ADDR;
    int i;
    if (off + 4 > GPU_VMEM_SIZE) {
      return;
    }

    if (mask == 0x0fu) {
//全字写单独走快路径
      uint32_t *dst = (uint32_t *)(void *)(gpu_vmem + off);
      *dst = data;
      gpu_dirty = 1;
      return;
    }

    for (i = 0; i < 4; i++) {
      if (mask & (1u << i)) {
        gpu_vmem[off + i] = (uint8_t)((data >> (8 * i)) & 0xffu);
        gpu_dirty = 1;
      }
    }
  }
}

//MMIO对外接口
int mmio_in_range(uint32_t addr_aligned) {
#if NPC_ENABLE_ASSERT
  assert((addr_aligned & 0x3u) == 0);
#endif

  return (addr_aligned == (MMIO_SERIAL_ADDR & ~0x3u)) ||
         (addr_aligned == (MMIO_RTC_LO_ADDR & ~0x3u)) ||
         (addr_aligned == (MMIO_RTC_HI_ADDR & ~0x3u)) ||
         (addr_aligned == (MMIO_KBD_ADDR & ~0x3u)) ||
         (addr_aligned == (MMIO_VGACTL_ADDR & ~0x3u)) ||
         (addr_aligned == (MMIO_SYNC_ADDR & ~0x3u)) ||
         fb_in_range(addr_aligned);
}

uint32_t mmio_read(uint32_t addr_aligned) {
#if NPC_ENABLE_ASSERT
  assert((addr_aligned & 0x3u) == 0);
#endif

  if (addr_aligned == (MMIO_VGACTL_ADDR & ~0x3u)) {
    return (GPU_WIDTH << 16) | GPU_HEIGHT;
  }

  if (addr_aligned == (MMIO_SYNC_ADDR & ~0x3u)) {
    return gpu_sync;
  }

  if (fb_in_range(addr_aligned)) {
    return fb_read_word(addr_aligned);
  }

  if (addr_aligned == (MMIO_RTC_LO_ADDR & ~0x3u)) {
//读LO时锁存当前时间,让后续HI/LO来自同一时刻
    rtc_latched_us = get_uptime_us();
    return (uint32_t)(rtc_latched_us & 0xffffffffu);
  }

  if (addr_aligned == (MMIO_RTC_HI_ADDR & ~0x3u)) {
    return (uint32_t)((rtc_latched_us >> 32) & 0xffffffffu);
  }

  if (addr_aligned == (MMIO_KBD_ADDR & ~0x3u)) {
//按bit15=keydown,低位=AM键码的语义返回
    return keyboard_poll_event();
  }

  if (addr_aligned == (MMIO_SERIAL_ADDR & ~0x3u)) {
//按UART语义返回1字节,无输入时为0xff
    return serial_poll_char();
  }

  return 0;
}

void mmio_write(uint32_t addr_aligned, uint32_t data, uint8_t mask) {
#if NPC_ENABLE_ASSERT
  assert((addr_aligned & 0x3u) == 0);
  assert((mask & 0xf0u) == 0);
#endif

  if (fb_in_range(addr_aligned)) {
    fb_write_word(addr_aligned, data, mask);
    return;
  }

  if (addr_aligned == (MMIO_SYNC_ADDR & ~0x3u)) {
//SYNC寄存器允许按字节写
    int i;
    for (i = 0; i < 4; i++) {
      if (mask & (1u << i)) {
        ((uint8_t *)&gpu_sync)[i] = (uint8_t)((data >> (8 * i)) & 0xffu);
      }
    }

    if (gpu_sync) {
//检测到SYNC被置位就触发刷新,然后清零
      int first_sync = !gpu_sync_seen;
      if (!gpu_sync_seen) {
        gpu_sync_seen = 1;
        fprintf(stderr, "[NPC][GPU] first SYNC write received\n");
      }

#if NPC_USE_SDL
      if (first_sync || gpu_dirty) {
        gpu_window_refresh();
        gpu_dirty = 0;
      }
#endif
      gpu_sync = 0;
    }
    return;
  }

  if (addr_aligned == (MMIO_SERIAL_ADDR & ~0x3u)) {
//按byte-mask提取字符,兼容sb/sh/sw写串口
    int i;
    for (i = 0; i < 4; i++) {
      if (mask & (1u << i)) {
        unsigned ch = (unsigned)((data >> (8 * i)) & 0xffu);
        serial_putc((char)ch);
      }
    }
    return;
  }

  (void)data;
  (void)mask;
}
