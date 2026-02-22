
#ifndef EXPR_H
#define EXPR_H

#include "common.h"

//表达式求值
uint32_t expr_eval(const char* expr, bool* success);//解析表达式并返回32位结果

#endif