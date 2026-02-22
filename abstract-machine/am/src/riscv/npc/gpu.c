#include <am.h>
#include <npc.h>
#include <sys/types.h>
#include <string.h>

#define GPU_SYNC_INTERVAL 4u

static uint32_t sync_pending = 0;//累计待提交的sync请求数

static inline void gpu_sync_maybe(int force) {
  if (force) {
    outl(SYNC_ADDR, 1);//强制立即刷新
    sync_pending = 0;
    return;
  }

  sync_pending++;
  if (sync_pending >= GPU_SYNC_INTERVAL) {
    outl(SYNC_ADDR, 1);//达到批量阈值后统一刷新
    sync_pending = 0;
  }
}

void __am_gpu_init() {//GPU初始化,清屏并请求刷新
  uint32_t v = inl(VGACTL_ADDR);//读取VGACTL寄存器,高16位是宽,低16位是高
  int w = v >> 16;//屏幕宽度
  int h = v & 0xffff;//屏幕高度

  if (w == 0) {
    w = 256;
  }
  if (h == 0) {
    h = 256;
  }

  uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;//帧缓冲起始地址
  for (int i = 0; i < w * h; i++) {
    fb[i] = 0;//逐像素清零,避免上电残留导致花屏
  }

  gpu_sync_maybe(1);//初始化阶段强制刷新一次
}

void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {//返回GPU静态能力与屏幕参数
  uint32_t v = inl(VGACTL_ADDR);//再次读取当前分辨率
  int w = v >> 16;//宽度
  int h = v & 0xffff;//高度

  *cfg = (AM_GPU_CONFIG_T){
    .present = true,
    .has_accel = false,
    .width = w,
    .height = h,
    .vmemsz = w*h*4,
  };
}

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {
  if (ctl->w == 0 || ctl->h == 0) {
    if (ctl->sync) {
      gpu_sync_maybe(1);//仅同步不绘制: 按AM语义立即刷新
    }
    return;
  }

  uint32_t v = inl(VGACTL_ADDR);//读整屏宽度用于行偏移计算
  int sw = v >> 16;//screen width

  uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;//目标帧缓冲
  uint32_t *p = (uint32_t *)ctl->pixels;//源像素块

  for (int j = 0; j < ctl->h; j++) {
    memcpy(fb + (ctl->y + j) * sw + ctl->x, p + j * ctl->w, (size_t)ctl->w * 4u);//逐行拷贝矩形区域
  }

  if (ctl->sync) {
    gpu_sync_maybe(0);//绘制路径做批量刷新,减少刷新频率
  }
}

void __am_gpu_status(AM_GPU_STATUS_T *status) {//查询GPU当前可用状态
  status->ready = true;//当前实现始终可接收绘制请求
}
