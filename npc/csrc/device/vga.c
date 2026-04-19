#include "device.h"
#include "map.h"
#include "mmio.h"

#include <stdint.h>
#include <string.h>

#define SCREEN_W 400u
#define SCREEN_H 300u

static uint32_t screen_width(void) {
  //屏宽
  return SCREEN_W;
}

static uint32_t screen_height(void) {
  return SCREEN_H;
}

static uint32_t screen_size(void) {
  return screen_width() * screen_height() * sizeof(uint32_t);
}

static void *vmem = NULL; //帧缓冲指针
static uint32_t *vgactl_port_base = NULL; //vga控制寄存器，[0]屏幕大小寄存器，故[1]同步寄存器

#if NPC_USE_SDL
#include <SDL2/SDL.h>

static SDL_Renderer *renderer = NULL;
static SDL_Texture *texture = NULL;
static int screen_inited = 0;

static void init_screen(void) {
  //屏幕初始化
  SDL_Window *window = NULL;
  if (screen_inited) {
    return;
  }
  screen_inited = 1;

  SDL_Init(SDL_INIT_VIDEO);
  SDL_CreateWindowAndRenderer((int)SCREEN_W * 2, (int)SCREEN_H * 2, 0, &window, &renderer);
  SDL_SetWindowTitle(window, "NPC");

  texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
      SDL_TEXTUREACCESS_STATIC, (int)SCREEN_W, (int)SCREEN_H);
  SDL_RenderPresent(renderer);
}

static inline void update_screen(void) {
  //更新
  if (!renderer || !texture) {
    return;
  }
  SDL_UpdateTexture(texture, NULL, vmem, (int)SCREEN_W * (int)sizeof(uint32_t));
  SDL_RenderClear(renderer);
  SDL_RenderCopy(renderer, texture, NULL, NULL);
  SDL_RenderPresent(renderer);
}
#else
static void init_screen(void) {
}

static inline void update_screen(void) {
}
#endif

void vga_update_screen(void) {
  //外部调用接口：每当vga同步寄存器被写入非0值时调用，触发屏幕更新并清零同步寄存器
  if (vgactl_port_base == NULL) {
    return;
  }

  if (vgactl_port_base[1]) {
    update_screen();
    vgactl_port_base[1] = 0; //清零同步寄存器
  }
}

void init_vga(void) {
  //把vga控制寄存器和帧缓冲映射到地址空间，并进行初始化
  vgactl_port_base = (uint32_t *)new_space(8);
  vgactl_port_base[0] = (screen_width() << 16) | screen_height(); //vga控制寄存器，[0]屏幕大小寄存器，故[1]同步寄存器
  add_mmio_map("vgactl", MMIO_VGACTL_ADDR, vgactl_port_base, 8, NULL);

  vmem = new_space((int)screen_size()); //分配帧缓冲空间
  add_mmio_map("vmem", MMIO_FB_ADDR, vmem, screen_size(), NULL); //把帧缓冲映射到MMIO地址空间

  init_screen();
  memset(vmem, 0, screen_size());
}
