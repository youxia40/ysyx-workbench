#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  const char *end = s;
    while (*end != '\0') {
        end++;
    }
    return end - s;
}

size_t strnlen(const char *s, size_t maxlen) {
    size_t len = 0;
    while (len < maxlen && s[len] != '\0') {
        len++;
    }
    return len;
}

char *strcpy(char *dst, const char *src) {
  if (src == NULL || dst == NULL) {
    printf("strcpy: NULL pointer passed");
    return NULL; // 返回NULL表示错误
  }
  else {
    char *p = dst;
    while (*src != '\0') {
      *p++ = *src++;
    }
    *p = '\0';
    return dst;
  }
}

char *strncpy(char *dst, const char *src, size_t n) {
  panic("Not implemented");
}

char *strcat(char *dst, const char *src) {
  if (src == NULL || dst == NULL) {
    printf("strcat: NULL pointer passed");
    return NULL; // 返回NULL表示错误
  }
  else {
    char *p = dst;
    while (*p != '\0') {
      p++;
    }
    while (*src != '\0') {
      *p++ = *src++;
    }
    *p = '\0';
    return dst;
  }
}

int strcmp(const char *s1, const char *s2) {
    if (s1 == NULL || s2 == NULL) {
        printf("strcmp: NULL pointer passed");
        return -1;                        // 返回-1表示错误
    }
    while (*s1 && *s2 && *s1 == *s2) {
        s1++;
        s2++;
    }

    return (signed char)*s1 - (signed char)*s2;
}

int strncmp(const char *s1, const char *s2, size_t n) {
  panic("Not implemented");
}

void *memset(void *s, int c, size_t n) {
  if (s == NULL ) {
    printf("memset: NULL pointer passed");
    return NULL; // 返回NULL表示错误
  }
  char *p = (char *)s;
  for(size_t i = 0; i<n; i++) {
    p[i] = (char)c;
  }
  return s;
}

void *memmove(void *dst, const void *src, size_t n) {
  panic("Not implemented");
}

void *memcpy(void *out, const void *in, size_t n) {
  char *d = out;
    const char *s = in;
    while (n--) {
        *d++ = *s++;                                            // 逐字节复制
    }
    return out;
}

int memcmp(const void *s1, const void *s2, size_t n) {
  const unsigned char *p1 = (const unsigned char *)s1;
    const unsigned char *p2 = (const unsigned char *)s2;
    for (size_t i = 0; i < n; i++) {
        if (p1[i] != p2[i]) {
            return (p1[i] > p2[i]) ? 1 : -1; // 返回第一个不同字节的差值符号
        }
    }
    return 0; // 所有字节相同
}

#endif
