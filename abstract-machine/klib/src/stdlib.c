#include <am.h>
#include <klib.h>
#include <klib-macros.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

static unsigned long int next = 1;

int rand(void) {
  next = next * 1103515245 + 12345;
  return (unsigned int)(next / 65536) % 32768;
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

  //跳过空白
  while (*nptr == ' ') nptr++;

  //处理符号
  if (*nptr == '-') {
    sign = -1;
    nptr++;
  } else if (*nptr == '+') {
    nptr++;
  }

  //转换数字
  while (*nptr >= '0' && *nptr <= '9') {
    result = result * 10 + (*nptr - '0');
    nptr++;
  }

  return sign * result;
}

static inline size_t align_up(size_t x, size_t a) {
  return (x + a - 1) & ~(a - 1);
}

void *malloc(size_t size) {
  if (size == 0) return NULL;

  //malloc返回地址需要对齐
  size = align_up(size, 8);

  extern Area heap;
  static char *addr = NULL;

  if (addr == NULL) {
    //第一次初始化：从heap.start开始
    assert(heap.start && heap.end);
    assert(heap.start < heap.end);
    addr = (char *)heap.start;
    addr = (char *)align_up((size_t)addr, 8);
  }

  //空间不足
  if (addr + size > (char *)heap.end) {
    return NULL;
  }

  void *ret = addr;
  addr += size;
  return ret;
}

void free(void *ptr) {
  (void)ptr;
}

#endif
