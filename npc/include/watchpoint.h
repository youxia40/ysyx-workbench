#ifndef WATCHPOINT_H
#define WATCHPOINT_H

#include "common.h"

//初始化监视点池
void wp_init();//初始化监视点池


//建监视点
void wp_new(const char *expr);//创建并注册一个监视点


//删除监视点
void wp_del(int id);//删除指定编号的监视点


//列出所有监视点
void wp_list();//列出当前所有监视点


//检查监视点是否触发，有变化返回则1
int wp_check();//检查表达式值变化并返回是否命中

#endif
