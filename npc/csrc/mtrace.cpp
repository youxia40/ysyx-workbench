#include "mtrace.h"
#include "common.h"

#include <cstdio>
#include <cstdint>

#if NPC_ENABLE_MTRACE


extern NPCContext npc_ctx;


//已打印的条数
static uint64_t mtrace_lines  = 0;

//是否已经静音
static bool     mtrace_silent = false;



//记录一次内存访问
void mtrace_log(int is_read, uint32_t addr, int len, uint32_t data) {

  //如果已经静音就直接返回
  if (mtrace_silent) return;



  //如果超过最大行数……
  if (mtrace_lines >= MTRACE_MAX_LINES) {
    printf("[mtrace] ... too many records, further logs are suppressed ...\n");
    mtrace_silent = true;
    return;
  }



  //按读/写类型过滤
  if (is_read && !MTRACE_LOG_READ)  return;
  if (!is_read && !MTRACE_LOG_WRITE) return;


  
  //按地址过滤
  if (addr < MTRACE_ADDR_LO || addr > MTRACE_ADDR_HI) {
    return;
  }

  const char* op = is_read ? "READ" : "WRITE";


  //打印当前pc 读写类型 地址 长度 数据
  printf("[mtrace] pc=0x%08x %sADDR=0x%08x LEN=%d DATA=0x%08x\n",
         npc_ctx.pc, op, addr, len, data);

  mtrace_lines++;
}



void mtrace_step(NPCContext* ctx) {
  (void)ctx;
}

#else  //NPC_ENABLE_MTRACE == 0



//关掉mtrace时的空实现
void mtrace_log(int is_read, uint32_t addr, int len, uint32_t data) {
  (void)is_read; (void)addr; (void)len; (void)data;
}

void mtrace_step(NPCContext* ctx) {
  (void)ctx;
}

#endif
