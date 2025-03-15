#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include"Vtop.h"
#include"verilated_fst_c.h"
#include"verilated.h"

VerilatedContext* contextp = NULL;
VerilatedFstC* tfp = NULL;
static Vtop* top;

void wave(){
  top->eval();
  contextp->timeInc(1);
  tfp->dump(contextp->time());
}

void init(){	//激励文件
	contextp = new VerilatedContext;
	tfp=new VerilatedFstC;		//创建跟踪对象                    
	top = new Vtop;	//实例化模块
	//波形文件
 	contextp->traceEverOn(true);
 	top->trace(tfp, 99);
 	tfp->open("waveform.fst");			//打开fst文件
}

int main(int argc, char** argv){
	init();
	top->en=0;top->x=0;wave();
		  top->x=1;wave();
		  top->x=2;wave();
		  top->x=4;wave();
		  top->x=8;wave();
		  top->x=16;wave();
		  top->x=32;wave();
		  top->x=64;wave();
		  top->x=128;wave();
	top->en=1;top->x=0;wave();
		  top->x=1;wave();
		  top->x=2;wave();
		  top->x=4;wave();
		  top->x=8;wave();
		  top->x=16;wave();
		  top->x=32;wave();
		  top->x=64;wave();
		  top->x=128;wave();	 	 	 	
	wave();
 	tfp->close();
	delete top;
	delete contextp;
	return 0;
}
