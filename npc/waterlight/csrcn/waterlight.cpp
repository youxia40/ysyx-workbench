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
int i=0;
int main(int argc, char** argv){
	init();
	while(i)
	{
	top->y=0;top->x0=0;wave();
		 top->x0=1;wave();
		 top->x0=2;wave();
		 top->x0=3;wave();
	top->y=1;top->x1=0;wave();
		 top->x1=1;wave();
		 top->x1=2;wave();
		 top->x1=3;wave();
	top->y=2;top->x2=0;wave();
		 top->x2=1;wave();
		 top->x2=2;wave();
		 top->x2=3;wave();
	top->y=3;top->x3=0;wave();
		 top->x3=1;wave();
		 top->x3=2;wave();
		 top->x3=3;wave();
	i++;
	}	 	 	 	
	wave();
 	tfp->close();
	delete top;
	delete contextp;
	return 0;
}
