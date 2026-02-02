#include "/home/pz40/ysyx-workbench/am-kernels/tests/am-tests/include/amtest.h"

#define W 400
#define H 300

static uint32_t color_buf[W][H];//颜色缓冲区，像素块
static bool has_kbd;    //键盘设备


void draw(uint32_t color) {
  // add code here
  for (int x = 0; x < W; x ++) {
    for (int y = 0; y < H; y ++) {
      color_buf[x][y] = color;//填充颜色缓冲区
    }
    
  }
  io_write(AM_GPU_FBDRAW, 0, 0, color_buf, W, H, false);//绘制到屏幕上
  io_write(AM_GPU_FBDRAW, 0, 0, NULL, 0, 0, true);//刷新屏幕  
}

uint32_t color_mem[] = {//变化的颜色
  0x000000, 0xff0000, 0x00ff00, 0x0000ff,
  0xffff00, 0xff00ff, 0x00ffff, 0xffffff
};


int main() {
  ioe_init();//初始化设备模型
  int sec=1;
  int pressed=0;//是否按下键盘
  uint64_t last_us=io_read(AM_TIMER_UPTIME).us;

  has_kbd  = io_read(AM_INPUT_CONFIG).present;

  while (1) {
    if (has_kbd) {
      while (1) {
        AM_INPUT_KEYBRD_T ev = io_read(AM_INPUT_KEYBRD);

        if (ev.keycode == AM_KEY_NONE) {
          break;              //队列空：直接退出
        }
        if (ev.keycode == AM_KEY_ESCAPE && ev.keydown) {
          return 0;
        }
        if (ev.keycode != AM_KEY_ESCAPE) {                 //只统计非ESC
          if (ev.keydown) {
            pressed++;
          }
            else if (pressed > 0){
              pressed--;                 //防止减成负数
            }
        }
      }
    }

    uint64_t interval = (pressed > 0) ? 50000 : 1000000;   //按住任意非ESC键快
    uint64_t now_us = io_read(AM_TIMER_UPTIME).us;

    if (now_us - last_us >= interval) {
      draw(color_mem[sec & 7]);
      sec++;
      last_us = now_us;
    }

  }
  return 0;
}
