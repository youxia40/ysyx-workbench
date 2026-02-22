#ifndef WAVE_H
#define WAVE_H

#include <cstdint>

class Vysyx_25040118_top;//前向声明顶层RTL模型类,避免直接包含verilated_fst_c.h导致编译依赖问题(报错找不到Vysyx_25040118_top定义)


void wave_init(Vysyx_25040118_top* top);//初始化FST波形文件
void wave_dump(uint64_t time);//在给定时间点写入波形
void wave_close();//关闭波形文件并释放资源


#endif
