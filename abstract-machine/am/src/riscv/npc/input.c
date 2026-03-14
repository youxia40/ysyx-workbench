#include <am.h>
#include <npc.h>

#define NPC_KEYDOWN_MASK 0x8000//键盘事件最高位:1=keydown,0=keyup

void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd) {
  uint32_t t = inl(KBD_ADDR);//读键盘事件寄存器,格式:bit15|am_keycode
  kbd->keydown = (t & NPC_KEYDOWN_MASK) ? 1 : 0;//提取按下/抬起状态
  kbd->keycode = t & ~NPC_KEYDOWN_MASK;//低位直接是AM键码
  if (kbd->keycode == 0) {
    kbd->keycode = AM_KEY_NONE;
  }
}
