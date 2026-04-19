#include "common.h"
#include "device.h"
#include "map.h"
#include "mmio.h"

#include <assert.h>

#define KEYDOWN_MASK 0x8000u

#if NPC_USE_SDL
#include <SDL2/SDL.h>

// Note that this is not the standard
#define NEMU_KEYS(f) \
  f(ESCAPE) f(F1) f(F2) f(F3) f(F4) f(F5) f(F6) f(F7) f(F8) f(F9) f(F10) f(F11) f(F12) \
  f(GRAVE) f(1) f(2) f(3) f(4) f(5) f(6) f(7) f(8) f(9) f(0) f(MINUS) f(EQUALS) f(BACKSPACE) \
  f(TAB) f(Q) f(W) f(E) f(R) f(T) f(Y) f(U) f(I) f(O) f(P) f(LEFTBRACKET) f(RIGHTBRACKET) f(BACKSLASH) \
  f(CAPSLOCK) f(A) f(S) f(D) f(F) f(G) f(H) f(J) f(K) f(L) f(SEMICOLON) f(APOSTROPHE) f(RETURN) \
  f(LSHIFT) f(Z) f(X) f(C) f(V) f(B) f(N) f(M) f(COMMA) f(PERIOD) f(SLASH) f(RSHIFT) \
  f(LCTRL) f(APPLICATION) f(LALT) f(SPACE) f(RALT) f(RCTRL) \
  f(UP) f(DOWN) f(LEFT) f(RIGHT) f(INSERT) f(DELETE) f(HOME) f(END) f(PAGEUP) f(PAGEDOWN)

#define NEMU_KEY_NAME(k) NEMU_KEY_##k,

enum {
  NEMU_KEY_NONE = 0,
  NEMU_KEYS(NEMU_KEY_NAME)
};

#define SDL_KEYMAP(k) keymap[SDL_SCANCODE_##k] = NEMU_KEY_##k;
static uint32_t keymap[256] = {};

static void init_keymap(void) {
  NEMU_KEYS(SDL_KEYMAP)
}

#define KEY_QUEUE_LEN 1024
static int key_queue[KEY_QUEUE_LEN] = {};
static int key_f = 0;
static int key_r = 0;

static void key_enqueue(uint32_t am_scancode) {
  key_queue[key_r] = (int)am_scancode;
  key_r = (key_r + 1) % KEY_QUEUE_LEN;
  assert(key_r != key_f);
}

static uint32_t key_dequeue(void) {
  //从键盘事件队列中取出一个按键事件，如果队列为空则返回NEMU_KEY_NONE
  uint32_t key = NEMU_KEY_NONE;
  if (key_f != key_r) {
    key = (uint32_t)key_queue[key_f];
    key_f = (key_f + 1) % KEY_QUEUE_LEN;
  }
  return key;
}

void send_key(uint8_t scancode, bool is_keydown) {
  //当有按键事件发生时，send_key函数会被调用，参数scancode表示按键的扫描码，is_keydown表示按键是按下还是释放
  if (!npc_ctx.stop && keymap[scancode] != NEMU_KEY_NONE) {
    //将扫描码转换为NEMU的按键码，并根据按键状态设置KEYDOWN_MASK
    uint32_t am_scancode = keymap[scancode] | (is_keydown ? KEYDOWN_MASK : 0);
    key_enqueue(am_scancode);
  }
}

#else

#define NEMU_KEY_NONE 0

static uint32_t key_dequeue(void) {//从键盘事件队列中取出一个按键事件，如果队列为空则返回NEMU_KEY_NONE
  return NEMU_KEY_NONE;
}

void send_key(uint8_t scancode, bool is_keydown) {
  (void)scancode;
  (void)is_keydown;
}

#endif

static uint32_t *i8042_data_port_base = NULL;

static void i8042_data_io_handler(uint32_t offset, int len, bool is_write) {
  assert(!is_write);
  assert(offset == 0);
  assert(len == 4);
  i8042_data_port_base[0] = key_dequeue();
}

void init_i8042(void) {
  i8042_data_port_base = (uint32_t *)new_space(4);
  i8042_data_port_base[0] = NEMU_KEY_NONE;
  add_mmio_map("keyboard", MMIO_KBD_ADDR, i8042_data_port_base, 4, i8042_data_io_handler);

#if NPC_USE_SDL
  init_keymap();
#endif
}
