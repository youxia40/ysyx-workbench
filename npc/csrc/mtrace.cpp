#include "mtrace.h"
#include "common.h"

#include <cstdio>
#include <cstdint>

#if NPC_ENABLE_MTRACE

// 使用全局上下文（只用来读当前 pc，方便调试）
extern NPCContext npc_ctx;

// ============= 地址过滤配置 =============
// 如果在 common.h 里已经定义了这些宏，这里的 #ifndef 不会覆盖你设置的值。
#ifndef MTRACE_ADDR_LO
#define MTRACE_ADDR_LO 0x00000000u
#endif

#ifndef MTRACE_ADDR_HI
#define MTRACE_ADDR_HI 0xffffffffu
#endif

// ============= 读/写过滤配置 =============
// 你已经在 common.h 里写了：
//   #define MTRACE_LOG_READ  0
//   #define MTRACE_LOG_WRITE 1
// 如果没写，就用这里的默认值（读写都开）。
#ifndef MTRACE_LOG_READ
#define MTRACE_LOG_READ 1
#endif

#ifndef MTRACE_LOG_WRITE
#define MTRACE_LOG_WRITE 1
#endif

// ============= 最大打印条数配置 =============
// 你在 common.h 里有：#define MTRACE_MAX_LINES 50
// 如果没写，就默认 200。
#ifndef MTRACE_MAX_LINES
#define MTRACE_MAX_LINES 200
#endif

// 已打印的条数 & 是否已经静音
static uint64_t mtrace_lines  = 0;
static bool     mtrace_silent = false;

// 记录一次内存访问
void mtrace_log(int is_read, uint32_t addr, int len, uint32_t data) {
  // 如果已经静音，直接返回
  if (mtrace_silent) return;

  // 如果超过最大行数，提示一次然后静音
  if (mtrace_lines >= MTRACE_MAX_LINES) {
    printf("[mtrace] ... too many records, further logs are suppressed ...\n");
    mtrace_silent = true;
    return;
  }

  // 按读/写类型过滤
  if (is_read && !MTRACE_LOG_READ)  return;
  if (!is_read && !MTRACE_LOG_WRITE) return;

  // 按地址过滤：不在区间内的直接丢掉
  if (addr < MTRACE_ADDR_LO || addr > MTRACE_ADDR_HI) {
    return;
  }

  const char* op = is_read ? "READ" : "WRITE";

  // 打印当前 pc + 读写类型 + 地址 + 长度 + 数据
  printf("[mtrace] pc=0x%08x %sADDR=0x%08x LEN=%d DATA=0x%08x\n",
         npc_ctx.pc, op, addr, len, data);

  mtrace_lines++;
}

// 目前真正记录都在 npc_mtrace_log -> mtrace_log 完成，这里保持空壳
void mtrace_step(NPCContext* ctx) {
  (void)ctx;
}

#else  // NPC_ENABLE_MTRACE == 0

// 关掉 mtrace 时的空实现，避免链接错误
void mtrace_log(int is_read, uint32_t addr, int len, uint32_t data) {
  (void)is_read; (void)addr; (void)len; (void)data;
}

void mtrace_step(NPCContext* ctx) {
  (void)ctx;
}

#endif
