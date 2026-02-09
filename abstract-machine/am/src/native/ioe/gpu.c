#include <am.h>
#include <SDL.h>
#include <fenv.h>

//#define MODE_800x600
#define WINDOW_W 800
#define WINDOW_H 600
#ifdef MODE_800x600
const int disp_w = WINDOW_W, disp_h = WINDOW_H;
#else
const int disp_w = 400, disp_h = 300;
#endif

#define FPS   60

#define RMASK 0x00ff0000
#define GMASK 0x0000ff00
#define BMASK 0x000000ff
#define AMASK 0x00000000

static SDL_Window *window = NULL;
static SDL_Surface *surface = NULL;

static Uint32 texture_sync(Uint32 interval, void *param) {
  SDL_BlitScaled(surface, NULL, SDL_GetWindowSurface(window), NULL);
  SDL_UpdateWindowSurface(window);
  return interval;
}

void __am_gpu_init() {
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
  window = SDL_CreateWindow("Native Application",
      SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
      WINDOW_W, WINDOW_H, SDL_WINDOW_OPENGL);
  surface = SDL_CreateRGBSurface(SDL_SWSURFACE, disp_w, disp_h, 32,
      RMASK, GMASK, BMASK, AMASK);
  SDL_AddTimer(1000 / FPS, texture_sync, NULL);
}

void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {//AM显示控制器信息, 可读出屏幕大小信息width和height.
                                            //另外AM假设系统在运行过程中, 屏幕大小不会发生变化.
  *cfg = (AM_GPU_CONFIG_T) {
    .present = true, .has_accel = false,
    .width = disp_w, .height = disp_h,
    .vmemsz = 0
  };
}

void __am_gpu_status(AM_GPU_STATUS_T *stat) {
  stat->ready = true;
}

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {//写入绘图信息向屏幕(x, y)坐标处绘制w*h的矩形图像,
                                            //图像像素按行优先方式存储在pixels中, 每个像素用32位整数
                                            //以00RRGGBB的方式描述颜色. 若sync为true, 则马上将帧缓冲中的内容同步到屏幕上.
  int x = ctl->x, y = ctl->y, w = ctl->w, h = ctl->h;
  if (w == 0 || h == 0) return;
  feclearexcept(-1);
  SDL_Surface *s = SDL_CreateRGBSurfaceFrom(ctl->pixels, w, h, 32, w * sizeof(uint32_t),
      RMASK, GMASK, BMASK, AMASK);
  SDL_Rect rect = { .x = x, .y = y };
  SDL_BlitSurface(s, NULL, surface, &rect);
  SDL_FreeSurface(s);
}
