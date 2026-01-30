#ifndef WATCHPOINT_H
#define WATCHPOINT_H

#include <cstdint>

//初始化监视点池
void wp_init();


//建监视点
void wp_new(const char *expr);


//删除监视点
void wp_del(int id);


//列出所有监视点
void wp_list();


//检查监视点是否触发，有变化返回则true
bool wp_check();

#endif
