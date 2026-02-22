#include "mmio.h"
#include "common.h"
#include <sys/time.h>
#include <stdlib.h>

#if NPC_USE_SDL
#include <SDL2/SDL.h>
#endif

#define GPU_WIDTH  400u
#define GPU_HEIGHT 300u
#define GPU_VMEM_SIZE (GPU_WIDTH * GPU_HEIGHT * 4u)
#define KBD_KEYDOWN_MASK 0x8000u

static char serial_buf[4096];//串口输出缓冲
static int serial_buf_len = 0;//串口缓冲当前长度

static int kbd_inited = 0;//键盘输入是否已初始化
static int kbd_ok = 0;//键盘输入当前是否可用
static char kbd_stty_old[128] = {0};//非SDL模式下保存原终端配置

static uint8_t gpu_vmem[GPU_VMEM_SIZE];//软件帧缓冲
static uint32_t gpu_sync = 0;//SYNC寄存器镜像
static int gpu_sync_seen = 0;//是否已经收到过首个SYNC
static int gpu_dirty = 0;//帧缓冲是否有改动(用于减少无效刷新)

static uint64_t rtc_latched_us = 0;//LO/HI配对读取时的锁存值
static uint64_t rtc_boot_us = 0;//启动时间戳

#if NPC_USE_SDL
static SDL_Window *gpu_window = NULL;//SDL窗口指针
static SDL_Renderer *gpu_renderer = NULL;//SDL渲染器指针
static SDL_Texture *gpu_texture = NULL;//SDL纹理指针
static int gpu_sdl_inited = 0;//SDL是否已初始化

static inline uint32_t sdl_key_to_raw(SDL_Keycode sym){//SDL键码转原始键值(字母/数字ASCII,其余按需映射)
  if ((sym >= SDLK_a && sym <= SDLK_z) || (sym >= SDLK_0 && sym <= SDLK_9)) {
    return (uint32_t)sym;
  }
  switch (sym) {//映射常用控制键和空格
    case SDLK_RETURN: return '\n';
    case SDLK_SPACE:  return ' ';
    case SDLK_ESCAPE: return 27;
    default: return 0;
  }
}

static void gpu_window_init(void){//初始化SDL窗口与渲染资源(懒初始化)
  if (gpu_sdl_inited) {
    return;
  }
  gpu_sdl_inited = 1;

  if (SDL_Init(SDL_INIT_VIDEO) != 0) {//初始化SDL视频子系统
    fprintf(stderr, "[NPC][GPU] SDL_Init failed: %s\n", SDL_GetError());
    return;
  }

  if (SDL_CreateWindowAndRenderer((int)GPU_WIDTH * 2, (int)GPU_HEIGHT * 2,0, &gpu_window, &gpu_renderer) != 0) {
    //创建窗口和渲染器失败时直接返回,保持GPU功能可用但不显示
    fprintf(stderr, "[NPC][GPU] SDL_CreateWindowAndRenderer failed: %s\n", SDL_GetError());
    return;
  }

  SDL_SetWindowTitle(gpu_window, "NPC");//标题
  gpu_texture = SDL_CreateTexture(gpu_renderer, SDL_PIXELFORMAT_ARGB8888,
                                  SDL_TEXTUREACCESS_STREAMING,
                                  (int)GPU_WIDTH, (int)GPU_HEIGHT);
  if (!gpu_texture) {
    fprintf(stderr, "[NPC][GPU] SDL_CreateTexture failed: %s\n", SDL_GetError());
    return;
  }

  fprintf(stderr, "[NPC][GPU] SDL window ready: %ux%u\n", GPU_WIDTH, GPU_HEIGHT);
}

static void gpu_window_refresh(void){//把软件帧缓冲提交到SDL窗口
  gpu_window_init();
  if (!gpu_renderer || !gpu_texture) {//如果渲染资源不可用就直接返回,保持GPU功能但不显示
    return;
  }

  SDL_UpdateTexture(gpu_texture, NULL, gpu_vmem, (int)GPU_WIDTH * 4);//更新整个纹理,每行GPU_WIDTH个像素每像素4字节(ARGB8888)
  SDL_RenderClear(gpu_renderer);//清空渲染目标
  SDL_RenderCopy(gpu_renderer, gpu_texture, NULL, NULL);//把纹理复制到渲染目标(窗口),不缩放
  SDL_RenderPresent(gpu_renderer);//显示渲染结果
}

static inline uint32_t keyboard_poll_event_sdl(void){//SDL键盘事件编码:bit15=1按下,bit15=0抬起,0=无事件
  gpu_window_init();
  //窗口事件与键盘事件共用同一轮SDL_PollEvent,这样可在无键盘输入时也及时处理窗口关闭事件



  SDL_Event ev;//SDL事件结构体,用于存储从事件队列中取出的事件信息
  while (SDL_PollEvent(&ev)) {//循环取出所有待处理事件
    if (ev.type == SDL_QUIT) {
      return KBD_KEYDOWN_MASK | 27u;
    }

    if (ev.type != SDL_KEYDOWN && ev.type != SDL_KEYUP) {
      continue;
    }
    if (ev.key.repeat) {
      continue;
    }

    uint32_t raw = sdl_key_to_raw(ev.key.keysym.sym);
    if (raw == 0) {
      continue;
    }

    if (ev.type == SDL_KEYDOWN) {
      return KBD_KEYDOWN_MASK | raw;
    }
    return raw;
  }

  return 0;
}
#endif

static inline void serial_flush(void){//把串口缓冲区刷到stdout
  if (serial_buf_len <= 0) {//没有数据需要刷新
    return;
  }

  //串口输出先缓冲后批量写出
  fwrite(serial_buf, 1, (size_t)serial_buf_len, stdout);
  fflush(stdout);//确保输出及时显示，位于缓冲区末尾的内容不会因为缓冲未满而延迟输出，stdio.h
  serial_buf_len = 0;//重置缓冲长度
}

static inline void serial_push_char(char ch){//推入一个字符,缓冲满时自动刷新
#if NPC_ENABLE_ASSERT
  assert(serial_buf_len >= 0 && serial_buf_len < (int)sizeof(serial_buf));
#endif
  serial_buf[serial_buf_len++] = ch;//把字符添加到缓冲区末尾
  if (serial_buf_len >= (int)sizeof(serial_buf)) {
    serial_flush();//缓冲区满了就刷新到stdout
  }
}

static inline int run_stty_cmd(const char *cmd) {//执行stty命令以设置或恢复终端属性
  return system(cmd);
}

static void keyboard_restore(void){//程序结束时恢复终端设置
  if (!kbd_ok) {
    return;
  }

  char cmd[192] = {0};//stty恢复命令
  if (kbd_stty_old[0] != '\0') {//如果保存了原终端配置就恢复，避免自定义配置丢失
    snprintf(cmd, sizeof(cmd), "stty %s 2>/dev/null", kbd_stty_old);
  } else {
    snprintf(cmd, sizeof(cmd), "stty sane 2>/dev/null");
  }
  (void)run_stty_cmd(cmd);
  //恢复后把kbd_ok清零，避免重复恢复导致状态抖动
  kbd_ok = 0;
}

static void keyboard_init_once(void){//初始化键盘输入(SDL直通,非SDL改stty)
  if (kbd_inited) {
    return;
  }
  kbd_inited = 1;//只初始化一次

#if NPC_USE_SDL
  kbd_ok = 1;//SDL模式直接标记键盘可用，后续通过事件轮询获取输入
  return;
#endif

  FILE *fp = popen("stty -g 2>/dev/null", "r");//执行stty命令获取当前终端配置以便后续恢复,输出重定向到/dev/null，stdio.h
  if (!fp) {
    return;
  }
  if (!fgets(kbd_stty_old, sizeof(kbd_stty_old), fp)) {//读取失败时直接返回，保持键盘功能但不修改终端设置
    pclose(fp);
    return;
  }
  pclose(fp);

  kbd_stty_old[strcspn(kbd_stty_old, "\r\n")] = '\0';//去掉行尾换行符，便于后续直接用在stty恢复命令中
  if (run_stty_cmd("stty -icanon -echo min 0 time 0 2>/dev/null") != 0) {
    //设置非规范模式和关闭回显，如果失败了就直接返回，保持键盘功能但不修改终端设置
    return;
  }

  atexit(keyboard_restore);//注册程序退出时的恢复函数，确保终端设置能被正确恢复，stdlib.h
  kbd_ok = 1;
}

static inline uint32_t keyboard_poll_event(void){//统一键盘事件读取(KEYDOWN:bit15=1,KEYUP:bit15=0,无事件:0)
  keyboard_init_once();
  if (!kbd_ok) {
    return 0;
  }

#if NPC_USE_SDL
  return keyboard_poll_event_sdl();//SDL模式通过事件轮询获取键盘输入，非SDL模式通过stty设置后直接读取stdin
#else
  int ch = getchar();//读取一个字符，如果没有输入则返回EOF
  if (ch == EOF) {//没有输入时返回0，表示无事件；如果读取失败也返回0，保持键盘功能但不报错
    clearerr(stdin);//清除EOF状态，准备下一次读取，stdio.h
    return 0;
  }
  return KBD_KEYDOWN_MASK | (uint32_t)(unsigned char)ch;//返回按下事件，包含原始字符值，非SDL模式不区分按下抬起，直接当作按下事件处理
#endif
}

static uint64_t get_time_us(void){//读取当前系统时间(微秒)
  struct timeval tv;//timeval结构体用于存储秒和微秒，sys/time.h

  gettimeofday(&tv, NULL);//获取当前时间
  return (uint64_t)tv.tv_sec * 1000000ull + (uint64_t)tv.tv_usec;
}

static uint64_t get_uptime_us(void){//读取从启动到当前的时间(微秒)
  uint64_t now = get_time_us();
  if (rtc_boot_us == 0) {
    //首次访问时记录boot基准，后续统一返回相对时间，便于与AM uptime语义对齐
    rtc_boot_us = now;
  }
  return now - rtc_boot_us;//返回相对时间
}

static inline int fb_in_range(uint32_t addr_aligned) {//判断地址是否命中帧缓冲区域，统一按word对齐地址匹配
  return addr_aligned >= MMIO_FB_ADDR && addr_aligned < (MMIO_FB_ADDR + GPU_VMEM_SIZE);
}

static inline uint32_t fb_read_word(uint32_t addr_aligned){//按4字节读取帧缓冲
#if NPC_ENABLE_ASSERT
  assert((addr_aligned & 0x3u) == 0);//调用方应传入word对齐地址
#endif
  if (!fb_in_range(addr_aligned)) {//地址不在帧缓冲范围内返回0，保持设备功能但不报错
    return 0;
  }

  uint32_t off = addr_aligned - MMIO_FB_ADDR;
  if (off + 4 > GPU_VMEM_SIZE) {//越界访问时返回0
    return 0;
  }

  return ((uint32_t)gpu_vmem[off + 0] << 0) |((uint32_t)gpu_vmem[off + 1] << 8) |
         ((uint32_t)gpu_vmem[off + 2] << 16) |((uint32_t)gpu_vmem[off + 3] << 24);//小端组为一个32位值
}

static inline void fb_write_word(uint32_t addr_aligned, uint32_t data, uint8_t mask){//按掩码写帧缓冲(mask=0x0f走整字快速路径)
#if NPC_ENABLE_ASSERT
  assert((addr_aligned & 0x3u) == 0);//调用方应传入word对齐地址
  assert((mask & 0xf0u) == 0);//写掩码仅允许低4位
#endif
  if (!fb_in_range(addr_aligned)) {
    return;
  }

  uint32_t off = addr_aligned - MMIO_FB_ADDR;//计算相对于帧缓冲起始地址的偏移
  if (off + 4 > GPU_VMEM_SIZE) {//越界访问直接丢弃
    return;
  }

  if (mask == 0x0fu) {
    uint32_t *dst = (uint32_t *)(void *)(gpu_vmem + off);//目标地址
    *dst = data;//全字写则直接覆盖
    gpu_dirty = 1;
    //全字写是最常见路径（如memcpy/fbdraw），走快速路径可减少逐字节分支开销
    return;
  }

  for (int i = 0; i < 4; i++) {
    if (mask & (1u << i)) {
      gpu_vmem[off + i] = (uint8_t)((data >> (8 * i)) & 0xffu);
      gpu_dirty = 1;
    }
  }
}

int mmio_in_range(uint32_t addr_aligned){//判断地址是否命中设备寄存器空间
#if NPC_ENABLE_ASSERT
  assert((addr_aligned & 0x3u) == 0);//MMIO匹配统一基于word对齐地址
#endif
  //按word对齐地址匹配，和DPI层aligned访问约定保持一致
  return (addr_aligned == (MMIO_SERIAL_ADDR & ~0x3u)) ||(addr_aligned == (MMIO_RTC_LO_ADDR & ~0x3u)) ||
  (addr_aligned == (MMIO_RTC_HI_ADDR & ~0x3u)) ||(addr_aligned == (MMIO_KBD_ADDR & ~0x3u)) ||
  (addr_aligned == (MMIO_VGACTL_ADDR & ~0x3u)) ||(addr_aligned == (MMIO_SYNC_ADDR & ~0x3u)) ||
  fb_in_range(addr_aligned);//帧缓冲地址也算MMIO地址，走设备路径处理
}

uint32_t mmio_read(uint32_t addr_aligned){//MMIO读入口
#if NPC_ENABLE_ASSERT
  assert((addr_aligned & 0x3u) == 0);
#endif
  if (addr_aligned == (MMIO_VGACTL_ADDR & ~0x3u)) {
    return (GPU_WIDTH << 16) | GPU_HEIGHT;//VGACTL:高16位宽,低16位高
  }

  if (addr_aligned == (MMIO_SYNC_ADDR & ~0x3u)) {//读SYNC寄存器返回当前镜像值
    return gpu_sync;
  }

  if (fb_in_range(addr_aligned)) {//帧缓冲区域按内存语义读取，直接返回对应地址的值
    return fb_read_word(addr_aligned);
  }

  if (addr_aligned == (MMIO_RTC_LO_ADDR & ~0x3u)) {
    rtc_latched_us = get_uptime_us();//读取LO时刷新锁存值,与随后HI配对
    //AM侧按low->high顺序读,这里在low时锁存可保证拼出的64位时间来自同一时刻

    return (uint32_t)(rtc_latched_us & 0xffffffffu);
  }

  if (addr_aligned == (MMIO_RTC_HI_ADDR & ~0x3u)) {
    //HI直接返回上一次锁存值的高32位,避免跨时刻读取导致高低位不一致
    return (uint32_t)((rtc_latched_us >> 32) & 0xffffffffu);
  }

  if (addr_aligned == (MMIO_KBD_ADDR & ~0x3u)) {//读取键盘事件
    return keyboard_poll_event();
  }

  return 0;
}

void mmio_write(uint32_t addr_aligned, uint32_t data, uint8_t mask){//MMIO写入口
#if NPC_ENABLE_ASSERT
  assert((addr_aligned & 0x3u) == 0);
  assert((mask & 0xf0u) == 0);
#endif
  if (fb_in_range(addr_aligned)) {
    //帧缓冲区域采用“内存语义”写入，不走寄存器语义
    fb_write_word(addr_aligned, data, mask);
    return;
  }

  if (addr_aligned == (MMIO_SYNC_ADDR & ~0x3u)) {
    //SYNC寄存器允许按字节写,这里把每个被mask选中的字节并入gpu_sync镜像
    for (int i = 0; i < 4; i++) {
      if (mask & (1u << i)) {
        ((uint8_t *)&gpu_sync)[i] = (uint8_t)((data >> (8 * i)) & 0xffu);
      }
    }

    if (gpu_sync) {
      int first_sync = !gpu_sync_seen;//首个SYNC保证首帧显示
      if (!gpu_sync_seen) {
        gpu_sync_seen = 1;//记录已经收到过SYNC的状态，后续不再打印首帧日志
        fprintf(stderr, "[NPC][GPU] first SYNC write received\n");
      }

#if NPC_USE_SDL
      if (first_sync || gpu_dirty) {//只有首帧或帧缓冲有改动时才刷新窗口
        gpu_window_refresh();
        gpu_dirty = 0;//刷新后清除
        //只有首帧或脏帧才刷新,减少空刷新带来的CPU开销
      }
#endif

      gpu_sync = 0;//写1触发后清零，下一次必须再次写SYNC才会触发刷新
    }
    return;
  }

  if (addr_aligned == (MMIO_SERIAL_ADDR & ~0x3u)) {//串口寄存器按内存语义写入，允许按字节写，未被mask选中的字节保持不变
    //串口寄存器按mask逐字节提取输出，可兼容sb/sh/sw等不同写宽指令

    for (int i = 0; i < 4; i++) {
      if (mask & (1u << i)) {//被mask选中的字节才处理
        unsigned ch = (unsigned)((data >> (8 * i)) & 0xffu);
        serial_push_char((char)ch);
        if (ch == '\n' || ch == '\r' || ch == '\b' || serial_buf_len >= 256) {
          //遇到行结束/退格或缓冲达到阈值时主动flush，平衡交互实时性与吞吐
          serial_flush();
        }
      }
    }
    return;
  }

  (void)data;
  (void)mask;
}
