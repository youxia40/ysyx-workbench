/***************************************************************************************
* Copyright (c) 2014-2024 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include <common.h>
#include <device/map.h>

#define SCREEN_W (MUXDEF(CONFIG_VGA_SIZE_800x600, 800, 400))
#define SCREEN_H (MUXDEF(CONFIG_VGA_SIZE_800x600, 600, 300))

static uint32_t screen_width() {//屏宽
  return MUXDEF(CONFIG_TARGET_AM, io_read(AM_GPU_CONFIG).width, SCREEN_W);
}

static uint32_t screen_height() {
  return MUXDEF(CONFIG_TARGET_AM, io_read(AM_GPU_CONFIG).height, SCREEN_H);
}

static uint32_t screen_size() {
  return screen_width() * screen_height() * sizeof(uint32_t);
}

static void *vmem = NULL;//帧缓冲指针
static uint32_t *vgactl_port_base = NULL;//vga控制寄存器，[0]屏幕大小寄存器，故[1]同步寄存器

#ifdef CONFIG_VGA_SHOW_SCREEN
#ifndef CONFIG_TARGET_AM
#include <SDL2/SDL.h>

static SDL_Renderer *renderer = NULL;//SDL渲染器指针
static SDL_Texture *texture = NULL;//SDL纹理指针

static void init_screen() {//屏幕初始化
  SDL_Window *window = NULL;
  char title[128];//窗口标题，包含ISA信息
  sprintf(title, "%s-NEMU", str(__GUEST_ISA__));
  SDL_Init(SDL_INIT_VIDEO);
  SDL_CreateWindowAndRenderer(//窗口尺寸根据配置选择400x300或800x600，与配置一致
      SCREEN_W * (MUXDEF(CONFIG_VGA_SIZE_400x300, 2, 1)),
      SCREEN_H * (MUXDEF(CONFIG_VGA_SIZE_400x300, 2, 1)),
      0, &window, &renderer);
  SDL_SetWindowTitle(window, title);//设置窗口标题
  texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
      SDL_TEXTUREACCESS_STATIC, SCREEN_W, SCREEN_H);
  SDL_RenderPresent(renderer);//显示初始空白窗口
}

static inline void update_screen() {//更新
  SDL_UpdateTexture(texture, NULL, vmem, SCREEN_W * sizeof(uint32_t));
  SDL_RenderClear(renderer);//清空渲染目标
  SDL_RenderCopy(renderer, texture, NULL, NULL);//把纹理复制到渲染目标(窗口)，不缩放
  SDL_RenderPresent(renderer);//显示渲染结果
}
#else
static void init_screen() {}

static inline void update_screen() {//AM平台通过写MMIO寄存器触发屏幕更新，直接把帧缓冲内容提交给环境
  io_write(AM_GPU_FBDRAW, 0, 0, vmem, screen_width(), screen_height(), true);
}
#endif
#endif

void vga_update_screen() {//外部调用接口：每当vga同步寄存器被写入非0值时调用，触发屏幕更新并清零同步寄存器
  // TODO: call `update_screen()` when the sync register is non-zero,
  // then zero out the sync register
  if (vgactl_port_base == NULL) return;
 
  if (vgactl_port_base[1]) {
    update_screen();
    vgactl_port_base[1] = 0;//清零同步寄存器
  }
}

void init_vga() {//把vga控制寄存器和帧缓冲映射到地址空间，并进行初始化
  vgactl_port_base = (uint32_t *)new_space(8);
  vgactl_port_base[0] = (screen_width() << 16) | screen_height();//vga控制寄存器，[0]屏幕大小寄存器，故[1]同步寄存器
#ifdef CONFIG_HAS_PORT_IO
  add_pio_map ("vgactl", CONFIG_VGA_CTL_PORT, vgactl_port_base, 8, NULL);
#else
  add_mmio_map("vgactl", CONFIG_VGA_CTL_MMIO, vgactl_port_base, 8, NULL);
#endif

  vmem = new_space(screen_size());//分配帧缓冲空间
  add_mmio_map("vmem", CONFIG_FB_ADDR, vmem, screen_size(), NULL);//把帧缓冲映射到MMIO地址空间
  IFDEF(CONFIG_VGA_SHOW_SCREEN, init_screen());
  IFDEF(CONFIG_VGA_SHOW_SCREEN, memset(vmem, 0, screen_size()));
}
