#include "trace/mtrace.h"

#if NPC_ENABLE_MTRACE

//已打印的条数
static uint64_t mtrace_lines = 0;

//是否已经静音
static bool mtrace_silent = false;



//记录一次内存访问
void mtrace_log(int is_read, uint32_t addr, int len, uint32_t data) {

  if (mtrace_silent) {
    return;
  }

  if (mtrace_lines >= MTRACE_MAX_LINES) {
    printf("[mtrace] ... too many records, further logs are suppressed ...\n");
    mtrace_silent = true;//达到上限后进入静音状态，后续日志直接返回
    return;
  }


  if ((is_read && !MTRACE_LOG_READ) || (!is_read && !MTRACE_LOG_WRITE)) {
    return;
  }
  if (addr < MTRACE_ADDR_LO || addr > MTRACE_ADDR_HI) {
    return;
  }

  const char* op = is_read ? "READ" : "WRITE";

  if (is_read) {
    //内存读追踪（直接打印）
    printf("[mtrace] %s  0x%08x, len=%d\n", op, addr, len);
  } else {
    //内存写追踪（直接打印）
    printf("[mtrace] %s 0x%08x, len=%d, data=0x%08x\n", op, addr, len, data);
  }

  //计数在最终打印后递增
  mtrace_lines++;
}

#else

//禁用时生成空函数
void mtrace_log(int is_read, uint32_t addr, int len, uint32_t data) { (void)is_read; (void)addr; (void)len; (void)data; }


#endif
