#include "wave.h"
#include "Vysyx_25040118_top.h"
#include <verilated.h>
#include <verilated_fst_c.h>
#include <cstdio>
#include <cstdlib>

static VerilatedFstC* tfp = nullptr;



void wave_init(Vysyx_25040118_top* top) {
    Verilated::traceEverOn(true);
    tfp = new VerilatedFstC;//创建FST波形对象
    //99层深度基本可覆盖大部分关键信号
    top->trace(tfp, 99);
    tfp->open("wave.fst");
}



//记录当前时刻的波形数据到FST文件
void wave_dump(uint64_t time) {
    if (tfp) {
        //time由主循环按“每个半周期+1”推进，保证波形时间轴与clk翻转一致
        tfp->dump(time);//记录当前时刻的波形数据到FST文件
    }
}



void wave_close() {
    if (tfp) {
        tfp->close();
        delete tfp;
        tfp = nullptr;//置空后可防止重复close/dump访问已释放对象
    }
}
