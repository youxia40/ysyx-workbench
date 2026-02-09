#include <am.h>
#include <nemu.h>
#include <sys/types.h>
#include <string.h>


#define SYNC_ADDR (VGACTL_ADDR + 4)//刷新寄存器

void __am_gpu_init() {
  uint32_t v = inl(VGACTL_ADDR);
  int w = v >> 16;
  int h = v & 0xffff;

  if (w == 0){
    w = 256;
  }
  if (h == 0){
    h = 256;
  }

  uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
  for (int i = 0; i < w * h; i++) {
    fb[i] = 0;   //清屏(原来fb[i]=i会出花屏色带）
  }

  outl(SYNC_ADDR, 1);

}

void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {//AM显示控制器信息, 可读出屏幕大小信息width和height.
                                            //另外AM假设系统在运行过程中, 屏幕大小不会发生变化.
                                            
  uint32_t v = inl(VGACTL_ADDR);
  int w = v >> 16;
  int h = v & 0xffff;

  *cfg = (AM_GPU_CONFIG_T){ 
    .present = true, 
    .has_accel = false, 
    .width = w, .height = h, 
    .vmemsz = w * h * 4 };
}

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {
  if (ctl->w == 0 || ctl->h == 0) { 
    if (ctl->sync) {
      outl(SYNC_ADDR, 1);  
    }
    return;
  }

  uint32_t v = inl(VGACTL_ADDR);
  int sw = v >> 16;//整屏宽度

  uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;//显存地址
  uint32_t *p  = (uint32_t *)ctl->pixels;//像素数据地址

  for (int j = 0; j < ctl->h; j++) {
    //    拷贝至      （x,y+j）     从  （w,j）         4w位
    memcpy(fb+(ctl->y+j)*sw+ctl->x,  p+j*ctl->w,  ctl->w*4);
  }

  if (ctl->sync) {
    outl(SYNC_ADDR, 1);//同步
  }
}

void __am_gpu_status(AM_GPU_STATUS_T *status) {
  status->ready = true;
}
