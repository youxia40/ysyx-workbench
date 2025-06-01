#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

// ======================== 辅助函数 ========================
static void reverse(char *str, int len) {
    for (int i = 0; i < len / 2; i++) {
        char tmp = str[i];
        str[i] = str[len - i - 1];
        str[len - i - 1] = tmp;
    }
}

static int itoa(int value, char *buf, int base, int sign) {
    if (base < 2 || base > 36) return 0;

    int is_negative = 0;
    char *ptr = buf;

    // 处理 value = 0 的特殊情况
    if (value == 0) {
        *ptr++ = '0';
        *ptr = '\0';
        return 1; // 返回长度1
    }

    if (sign && base == 10 && value < 0) {
        is_negative = 1;
        value = -value;
    }

    while (value != 0) {
        int digit = value % base;
        *ptr++ = (digit < 10) ? '0' + digit : 'a' + digit - 10;
        value /= base;
    }

    if (is_negative) *ptr++ = '-';
    *ptr = '\0';
    reverse(buf, ptr - buf);
    return ptr - buf;
}

static int utoa(unsigned int value, char *buf, int base) {
    if (base < 2 || base > 36) return 0;

    char *ptr = buf;

    // 处理 value = 0 的特殊情况
    if (value == 0) {
        *ptr++ = '0';
        *ptr = '\0';
        return 1; // 返回长度1
    }

    while (value != 0) {
        unsigned int digit = value % base;
        *ptr++ = (digit < 10) ? '0' + digit : 'a' + digit - 10;
        value /= base;
    }

    *ptr = '\0';
    reverse(buf, ptr - buf);
    return ptr - buf;
}

// ======================== 核心格式化函数 ========================
static int vsnprintf_internal(char *buf, size_t size, const char *fmt, va_list ap) {
    char *start = buf;
    const char *end = (size == 0) ? (char*)(SIZE_MAX) : (buf + size);

    while (*fmt && buf < end) {
        if (*fmt != '%') {
            *buf++ = *fmt++;
            continue;
        }

        fmt++; // 跳过 '%'
        switch (*fmt) {
            case 's': {
                char *s = va_arg(ap, char*);
                if (!s) s = "(null)";
                size_t len = 0;
                while (s[len] && buf + len < end) len++;
                memcpy(buf, s, len);
                buf += len;
                break;
            }
            case 'd':
            case 'i': {
                int num = va_arg(ap, int);
                char num_buf[32];
                int len = itoa(num, num_buf, 10, 1);
                if (buf + len >= end) len = end - buf;
                memcpy(buf, num_buf, len);
                buf += len;
                break;
            }
            case 'u': {
                unsigned int num = va_arg(ap, unsigned int);
                char num_buf[32];
                int len = utoa(num, num_buf, 10);
                if (buf + len >= end) len = end - buf;
                memcpy(buf, num_buf, len);
                buf += len;
                break;
            }
            case 'x':
            case 'X': {
                unsigned int num = va_arg(ap, unsigned int);
                char num_buf[32];
                int len = utoa(num, num_buf, 16);
                if (buf + len >= end) len = end - buf;
                memcpy(buf, num_buf, len);
                buf += len;
                break;
            }
            case 'c': {
                char c = (char)va_arg(ap, int);
                if (buf < end) *buf++ = c;
                break;
            }
            case '%': {
                if (buf < end) *buf++ = '%';
                break;
            }
            default: {
                if (buf < end) *buf++ = '%';
                if (buf < end) *buf++ = *fmt;
                break;
            }
        }
        fmt++;
    }

    // 安全终止缓冲区
    if (size > 0) {
        if (buf < end) {
            *buf = '\0';
        } else {
            start[size - 1] = '\0';                     // 缓冲区满时强制截断
        }
    }
    return buf - start;
}

// ======================== 标准函数接口 ========================
int vsprintf(char *str, const char *format, va_list ap) {
    return vsnprintf_internal(str, SIZE_MAX, format, ap);
}

int sprintf(char *str, const char *format, ...) {
    va_list ap;
    va_start(ap, format);
    int ret = vsnprintf_internal(str, SIZE_MAX, format, ap);
    va_end(ap);
    return ret;
}

int vsnprintf(char *str, size_t size, const char *format, va_list ap) {
    return vsnprintf_internal(str, size, format, ap);
}

int snprintf(char *str, size_t size, const char *format, ...) {
    va_list ap;
    va_start(ap, format);
    int ret = vsnprintf_internal(str, size, format, ap);
    va_end(ap);
    return ret;
}

int vprintf(const char *format, va_list ap) {
    char buf[1024];
    int len = vsnprintf_internal(buf, sizeof(buf), format, ap);
    for (int i = 0; i < len; i++) putch(buf[i]);
    return len;
}

int printf(const char *format, ...) {
    va_list ap;
    va_start(ap, format);
    int ret = vprintf(format, ap);
    va_end(ap);
    return ret;
}

#endif