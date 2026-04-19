#include "trace/dtrace.h"

#if NPC_ENABLE_DTRACE

#include <stdio.h>
#include <string.h>

static bool dtrace_enabled = true;  //全局开关

static inline bool dtrace_skip_dev(const char *dev) {
  //过滤掉串口设备的访问日志，dev是设备名称字符串命名为"serial"
  return dev != NULL && strcmp(dev, "serial") == 0;
}

//启用/禁用追踪
void dtrace_enable(bool enable) {
  dtrace_enabled = enable;
}

//设备读追踪(直接打印）
void dtrace_read(uint32_t addr, int len, const char *dev) {
  if (dtrace_enabled && !dtrace_skip_dev(dev)) {
    printf("[dtrace] READ  %-12s 0x%08x, len=%d\n",
           dev ? dev : "(null)", addr, len);
  }
}

//内存写追踪（直接打印）
void dtrace_write(uint32_t addr, int len, uint32_t data, const char *dev) {
  if (dtrace_enabled && !dtrace_skip_dev(dev)) {
    printf("[dtrace] WRITE %-12s 0x%08x, len=%d, data=0x%08x\n",
           dev ? dev : "(null)", addr, len, data);
  }
}

#else

//禁用时生成空函数
void dtrace_enable(bool enable) {
  (void)enable;
}

void dtrace_read(uint32_t addr, int len, const char *dev) {
  (void)addr;
  (void)len;
  (void)dev;
}

void dtrace_write(uint32_t addr, int len, uint32_t data, const char *dev) {
  (void)addr;
  (void)len;
  (void)data;
  (void)dev;
}

#endif
