#include <am.h>
#include <klib.h>
#include <klib-macros.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

static unsigned long int next = 1;

int rand(void) {
  next = next * 1103515245 + 12345;
  return (unsigned int)(next/65536) % 32768;
}

void srand(unsigned int seed) {
  next = seed;
}

int abs(int x) {
  return (x < 0 ? -x : x);
}

int atoi(const char* nptr) {
  if (!nptr) return 0;
  
  int sign = 1;
  int result = 0;
  
  // 跳过前导空白
  while (*nptr == ' ') nptr++;
  
  // 处理符号
  if (*nptr == '-') {
    sign = -1;
    nptr++;
  } else if (*nptr == '+') {
    nptr++;
  }
  
  // 转换数字
  while (*nptr >= '0' && *nptr <= '9') {
    result = result * 10 + (*nptr - '0');
    nptr++;
  }
  
  return sign * result;
}

// 简化内存管理实现
static char mem_pool[128 * 1024]; // 128KB内存池
static size_t pool_used = 0;

void *malloc(size_t size) {
  // 在原生环境中不调用panic()
#if !(defined(__ISA_NATIVE__) && defined(__NATIVE_USE_KLIB__))
  return NULL;
#endif

  if (size == 0) return NULL;
  
  // 检查是否有足够空间
  if (pool_used + size > sizeof(mem_pool)) {
    return NULL;
  }
  
  // 分配内存
  void *ptr = mem_pool + pool_used;
  pool_used += size;
  return ptr;
}

void free(void *ptr) {
  // 在简化实现中，不实际释放内存
  (void)ptr;
}

#endif