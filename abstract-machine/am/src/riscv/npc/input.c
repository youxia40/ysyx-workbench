#include <am.h>
#include <npc.h>

#define NPC_KEYDOWN_MASK 0x8000//键盘事件最高位:1=keydown,0=keyup


static int decode_keycode(uint32_t k) {//把NPC原始键值(ASCII/控制符)映射到AM_KEY_*编码
  switch (k) {
    case 27: return AM_KEY_ESCAPE;
    case '\n': case '\r': return AM_KEY_RETURN;
    case ' ': return AM_KEY_SPACE;


    case 'a': case 'A': return AM_KEY_A;
    case 'b': case 'B': return AM_KEY_B;
    case 'c': case 'C': return AM_KEY_C;
    case 'd': case 'D': return AM_KEY_D;
    case 'e': case 'E': return AM_KEY_E;
    case 'f': case 'F': return AM_KEY_F;
    case 'g': case 'G': return AM_KEY_G;
    case 'h': case 'H': return AM_KEY_H;
    case 'i': case 'I': return AM_KEY_I;
    case 'j': case 'J': return AM_KEY_J;
    case 'k': case 'K': return AM_KEY_K;
    case 'l': case 'L': return AM_KEY_L;
    case 'm': case 'M': return AM_KEY_M;
    case 'n': case 'N': return AM_KEY_N;
    case 'o': case 'O': return AM_KEY_O;
    case 'p': case 'P': return AM_KEY_P;
    case 'q': case 'Q': return AM_KEY_Q;
    case 'r': case 'R': return AM_KEY_R;
    case 's': case 'S': return AM_KEY_S;
    case 't': case 'T': return AM_KEY_T;
    case 'u': case 'U': return AM_KEY_U;
    case 'v': case 'V': return AM_KEY_V;
    case 'w': case 'W': return AM_KEY_W;
    case 'x': case 'X': return AM_KEY_X;
    case 'y': case 'Y': return AM_KEY_Y;
    case 'z': case 'Z': return AM_KEY_Z;
    case '0': return AM_KEY_0;
    case '1': return AM_KEY_1;
    case '2': return AM_KEY_2;  
    case '3': return AM_KEY_3;
    case '4': return AM_KEY_4;
    case '5': return AM_KEY_5;
    case '6': return AM_KEY_6;
    case '7': return AM_KEY_7;
    case '8': return AM_KEY_8;
    case '9': return AM_KEY_9;

    
    default: return AM_KEY_NONE;
  }
}

void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd) {
  uint32_t t = inl(KBD_ADDR);//读键盘事件寄存器,格式:bit15|raw_key
  kbd->keydown = (t & NPC_KEYDOWN_MASK) ? 1 : 0;//提取按下/抬起状态
  kbd->keycode = decode_keycode(t & ~NPC_KEYDOWN_MASK);//提取低位原始键值并转换成AM键码
  if (kbd->keycode == AM_KEY_NONE) {
    kbd->keydown = 0;//未知键不作为有效按键事件上报
  }
}
