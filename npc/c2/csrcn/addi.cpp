#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "Vysyx_25040118_top.h"
#include "verilated_fst_c.h"
#include "verilated.h"

VerilatedContext* contextp = NULL;
VerilatedFstC* tfp = NULL;
Vysyx_25040118_top* top = NULL;                                  // 对应 Verilog 顶层模块名


static bool ebreak_triggered = false;  // 用于检测是否触发了 ebreak
extern "C" void ebreak_trigger() {
    printf("***Ebreak triggered, stop...***\n");
    ebreak_triggered = true;
}


int main(int argc, char** argv){
	//激励文件
	contextp = new VerilatedContext;
	tfp = new VerilatedFstC;		//创建跟踪对象                    
	top = new Vysyx_25040118_top{contextp};	//实例化模块
	//波形文件
 	
 	contextp->traceEverOn(true);
 	top->trace(tfp, 99);
 	tfp->open("waveform.fst");			//打开fst文件

 	int max_cycles = 100;                                        // 默认仿真周期数
    if (argc > 1) {
        max_cycles = atoi(argv[1]);                         // 通过命令行参数调整
    }

    top->clk = 0;		                                                    //时钟信号
    top->rst = 1;		                                                        //复位信号

     // 复位序列
     for (int i=0; i<10; i++) {                          // 2个周期复位
         top->clk = !top->clk;
         top->eval();
         tfp->dump(contextp->time());                               //记录波形
         contextp->timeInc(1);                                              //更新时间
     }
     top->rst = 0;

    // 主仿真循环
    int cycle = 1;
    printf("\n");
    printf("Cycle 0: PC=0x%08x | ins=0x%08x | rd_addr=%d | wen=%d", top->pc, top->ins, top->rd_addr, top->wen);                 //打印初始状态，即m[0]
    printf(" | Write x%02d=0x%08x\n", top->reg_waddr, top->reg_wdata);
    while (cycle < max_cycles && !ebreak_triggered) {
        top->clk = 1;
        top->eval();
        tfp->dump(contextp->time());
        contextp->timeInc(1);

        printf("Cycle%2d: PC=0x%08x | ins=0x%08x | rd_addr=%d | wen=%d", cycle, top->pc, top->ins, top->rd_addr, top->wen);
        if (top->reg_waddr != 0) {
            printf(" | Write x%02d=0x%08x", top->reg_waddr, top->reg_wdata);
        }
        printf("\n");

        if (ebreak_triggered == true) {
            break;
        }

        top->clk = 0;
        top->eval();
        tfp->dump(contextp->time());
        contextp->timeInc(1);

        cycle++;
    }
 	tfp->close();
	delete top;
	delete contextp;
	return 0;
}