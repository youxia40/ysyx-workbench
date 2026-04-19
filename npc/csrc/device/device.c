#include "device.h"
#include "common.h"
#include "map.h"

#include <stdint.h>
#include <sys/time.h>

#if NPC_USE_SDL
#include <SDL2/SDL.h>
#endif

#define TIMER_HZ 60u

static uint64_t get_time_us(void) {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return (uint64_t)tv.tv_sec * 1000000ull + (uint64_t)tv.tv_usec;
}

void device_update(void) {
  //设备更新函数
  static uint64_t last = 0;
  uint64_t now = get_time_us();

  if (now - last < 1000000ull / TIMER_HZ) {
    return;
  }
  last = now;

  vga_update_screen(); //更新屏幕

#if NPC_USE_SDL
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    //处理SDL事件
    switch (event.type) {
      case SDL_QUIT:
        npc_ctx.stop = true;
        if (npc_ctx.stop_reason == NULL) {
          npc_ctx.stop_reason = (char *)"SDL_QUIT";
        }
        break;

      case SDL_KEYDOWN:
      case SDL_KEYUP: {
        uint8_t k = (uint8_t)event.key.keysym.scancode;
        bool is_keydown = (event.type == SDL_KEYDOWN);
        send_key(k, is_keydown);

        if (is_keydown) {
          SDL_Keycode sym = event.key.keysym.sym;
          if (sym == SDLK_RETURN || sym == SDLK_KP_ENTER) {
            serial_feed_input('\r');
          } else if (sym == SDLK_BACKSPACE) {
            serial_feed_input('\b');
          } else if (sym == SDLK_TAB) {
            serial_feed_input('\t');
          } else if (sym >= 32 && sym <= 126) {
            serial_feed_input((uint8_t)sym);
          }
        }

        break;
      }

      default:
        break;
    }
  }
#endif
}

void init_device(void) {
  //初始化设备，包括IO映射和各类外设
  init_map();

  init_serial();
  init_timer(); //初始化定时器设备,会在monitor.c里调用add_mmio_map把它映射到地址空间
  init_vga();
  init_i8042();
}
