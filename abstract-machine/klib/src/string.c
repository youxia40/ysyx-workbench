#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>
#include <stddef.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
    if (!s) return 0;
    const char *end = s;
    while (*end != '\0') {
        end++;
    }
    return end - s;
}

size_t strnlen(const char *s, size_t maxlen) {
    if (!s) return 0;
    size_t len = 0;
    while (len < maxlen && s[len] != '\0') {
        len++;
    }
    return len;
}

char *strcpy(char *dst, const char *src) {
    if (!src || !dst) return NULL;
    
    char *p = dst;
    while (*src != '\0') {
        *p++ = *src++;
    }
    *p = '\0';
    return dst;
}

char *strncpy(char *dst, const char *src, size_t n) {
    if (!src || !dst || n == 0) return NULL;
    
    char *p = dst;
    size_t i = 0;
    
    // 复制最多 n 个字符
    while (i < n && *src != '\0') {
        *p++ = *src++;
        i++;
    }
    
    // 如果源字符串不足 n 个字符，填充 '\0'
    while (i < n) {
        *p++ = '\0';
        i++;
    }
    
    return dst;
}

char *strcat(char *dst, const char *src) {
    if (!src || !dst) return NULL;
    
    // 找到 dst 的结尾
    char *p = dst;
    while (*p != '\0') {
        p++;
    }
    
    // 复制 src，但不超过剩余空间
    while (*src != '\0') {
        *p++ = *src++;
    }
    
    *p = '\0';
    return dst;
}

int strcmp(const char *s1, const char *s2) {
    if (!s1 || !s2) return -1;
    
    while (*s1 && *s2 && *s1 == *s2) {
        s1++;
        s2++;
    }
    
    return (signed char)*s1 - (signed char)*s2;
}

int strncmp(const char *s1, const char *s2, size_t n) {
    if (n == 0) return 0;
    
    while (n-- > 0) {
        unsigned char c1 = (unsigned char)*s1++;
        unsigned char c2 = (unsigned char)*s2++;
        
        if (c1 != c2) {
            return (int)c1 - (int)c2;
        }
        if (c1 == '\0') {
            break;
        }
    }
    return 0;
}

void *memset(void *s, int c, size_t n) {
    if (!s) return NULL;
    
    unsigned char *p = (unsigned char *)s;
    while (n-- > 0) {
        *p++ = (unsigned char)c;
    }
    return s;
}

void *memmove(void *dst, const void *src, size_t n) {
    if (!dst || !src || n == 0) return dst;
    
    unsigned char *d = (unsigned char *)dst;
    const unsigned char *s = (const unsigned char *)src;
    
    // 处理重叠情况
    if (s < d && d < s + n) {
        // 从后往前复制
        d += n - 1;
        s += n - 1;
        while (n-- > 0) {
            *d-- = *s--;
        }
    } else {
        // 从前往后复制
        while (n-- > 0) {
            *d++ = *s++;
        }
    }
    
    return dst;
}

void *memcpy(void *dst, const void *src, size_t n) {
    return memmove(dst, src, n);
}

int memcmp(const void *s1, const void *s2, size_t n) {
    if (!s1 || !s2) return -1;
    if (n == 0) return 0;
    
    const unsigned char *p1 = (const unsigned char *)s1;
    const unsigned char *p2 = (const unsigned char *)s2;
    
    while (n-- > 0) {
        if (*p1 != *p2) {
            return (*p1 > *p2) ? 1 : -1;
        }
        p1++;
        p2++;
    }
    
    return 0;
}

#endif