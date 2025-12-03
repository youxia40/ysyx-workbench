#include "wave.h"
#include "common.h"
#include "Vysyx_25040118_top.h"
#include <verilated.h>
#include <verilated_fst_c.h>
#include <cstdio>
#include <cstdlib>

static VerilatedFstC* tfp = nullptr;

// 初始化波形
void wave_init(Vysyx_25040118_top* top) {
    Verilated::traceEverOn(true);
    tfp = new VerilatedFstC;
    top->trace(tfp, 99);
    tfp->open("wave.fst");
}

// 转储波形
void wave_dump(uint64_t time) {
    if (tfp) {
        tfp->dump(time);
    }
}

// 关闭波形
void wave_close() {
    if (tfp) {
        tfp->close();
        delete tfp;
        tfp = nullptr;
    }
}
