#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

// ======================== 辅助函数 ========================
static void reverse(char *str, size_t len) {
    size_t start = 0;
    size_t end = len - 1;
    while (start < end) {
        char tmp = str[start];
        str[start] = str[end];
        str[end] = tmp;
        start++;
        end--;
    }
}

// 整数转字符串 (安全版本)
static int safe_itoa(long value, char *buf, size_t buf_size, int base, int sign) {
    if (buf_size < 1) return -1;
    if (base < 2 || base > 36) return -1;
    
    size_t i = 0;
    int is_negative = 0;
    
    // 处理符号
    if (sign && value < 0) {
        is_negative = 1;
        value = -value;
    }
    
    // 处理 0 的情况
    if (value == 0) {
        if (buf_size < 2) return -1;
        buf[0] = '0';
        buf[1] = '\0';
        return 1;
    }
    
    // 转换为字符串 (逆序)
    while (value != 0 && i < buf_size - 1) {
        long digit = value % base;
        buf[i++] = (digit < 10) ? '0' + digit : 'a' + digit - 10;
        value /= base;
    }
    
    // 添加符号
    if (is_negative && i < buf_size - 1) {
        buf[i++] = '-';
    }
    
    // 添加结束符
    if (i >= buf_size - 1) {
        // 缓冲区不足
        return -1;
    }
    buf[i] = '\0';
    
    // 反转字符串
    reverse(buf, i);
    
    return i;
}

// 无符号整数转字符串 (安全版本)
static int safe_utoa(unsigned long value, char *buf, size_t buf_size, int base) {
    if (buf_size < 1) return -1;
    if (base < 2 || base > 36) return -1;
    
    size_t i = 0;
    
    // 处理 0 的情况
    if (value == 0) {
        if (buf_size < 2) return -1;
        buf[0] = '0';
        buf[1] = '\0';
        return 1;
    }
    
    // 转换为字符串 (逆序)
    while (value != 0 && i < buf_size - 1) {
        unsigned long digit = value % base;
        buf[i++] = (digit < 10) ? '0' + digit : 'a' + digit - 10;
        value /= base;
    }
    
    // 添加结束符
    if (i >= buf_size - 1) {
        // 缓冲区不足
        return -1;
    }
    buf[i] = '\0';
    
    // 反转字符串
    reverse(buf, i);
    
    return i;
}

// ======================== 核心格式化函数 ========================
static int vsnprintf_internal(char *buf, size_t size, const char *fmt, va_list ap) {
    size_t pos = 0;
    
    while (*fmt && pos < size) {
        if (*fmt != '%') {
            buf[pos++] = *fmt++;
            continue;
        }
        
        fmt++; // 跳过 '%'
        if (*fmt == '\0') break;
        
        char specifier = *fmt++;
        
        switch (specifier) {
            case 's': {
                char *s = va_arg(ap, char*);
                if (!s) s = "(null)";
                
                // 安全复制字符串
                size_t len = strnlen(s, size - pos - 1);
                strncpy(buf + pos, s, len);
                pos += len;
                break;
            }
            case 'd':
            case 'i': {
                int num = va_arg(ap, int);
                char num_buf[32];
                int len = safe_itoa(num, num_buf, sizeof(num_buf), 10, 1);
                if (len > 0) {
                    size_t to_copy = len;
                    if (pos + to_copy > size - 1) {
                        to_copy = size - pos - 1;
                    }
                    strncpy(buf + pos, num_buf, to_copy);
                    pos += to_copy;
                }
                break;
            }
            case 'u': {
                unsigned int num = va_arg(ap, unsigned int);
                char num_buf[32];
                int len = safe_utoa(num, num_buf, sizeof(num_buf), 10);
                if (len > 0) {
                    size_t to_copy = len;
                    if (pos + to_copy > size - 1) {
                        to_copy = size - pos - 1;
                    }
                    strncpy(buf + pos, num_buf, to_copy);
                    pos += to_copy;
                }
                break;
            }
            case 'x':
            case 'X': {
                unsigned int num = va_arg(ap, unsigned int);
                char num_buf[32];
                int len = safe_utoa(num, num_buf, sizeof(num_buf), 16);
                if (len > 0) {
                    size_t to_copy = len;
                    if (pos + to_copy > size - 1) {
                        to_copy = size - pos - 1;
                    }
                    strncpy(buf + pos, num_buf, to_copy);
                    pos += to_copy;
                }
                break;
            }
            case 'c': {
                char c = (char)va_arg(ap, int);
                if (pos < size - 1) {
                    buf[pos++] = c;
                }
                break;
            }
            case '%': {
                if (pos < size - 1) {
                    buf[pos++] = '%';
                }
                break;
            }
            default: {
                // 忽略不支持的格式化符号
                break;
            }
        }
    }
    
    // 安全终止缓冲区
    if (size > 0) {
        if (pos < size) {
            buf[pos] = '\0';
        } else {
            buf[size - 1] = '\0';
        }
    }
    
    return pos;
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
    // 简化实现
    char buf[512];
    int len = vsnprintf_internal(buf, sizeof(buf), format, ap);
    for (int i = 0; i < len && buf[i] != '\0'; i++) {
        putch(buf[i]);
    }
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