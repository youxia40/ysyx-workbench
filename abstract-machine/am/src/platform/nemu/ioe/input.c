#include <am.h>
#include <nemu.h>

#define KEYDOWN_MASK 0x8000//按键按下的标志位，用来区分按键是按下还是释放，按下时该位为1，释放时该位为0

void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd) {
  uint32_t t = inl(KBD_ADDR);//t的格式：最高位是按键状态位，剩余位是按键扫描码
  kbd->keydown = (t & KEYDOWN_MASK) ? 1 : 0;//只保留最高位，判断按键是按下还是释放
  kbd->keycode = t & ~KEYDOWN_MASK;//根据KEYDOWN_MASK位的值获取按键的扫描码，存储在keycode字段中
  if (kbd->keycode==0) {
    kbd->keycode = AM_KEY_NONE;
  }
}
