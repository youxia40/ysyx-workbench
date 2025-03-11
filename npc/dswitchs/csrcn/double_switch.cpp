#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include"Vtop.h"
#include"verilated_fst_c.h"
#include"verilated.h"
int main(int argc, char** argv){
	//激励文件
	VerilatedContext* contextp = new VerilatedContext;                        
	contextp->commandArgs(argc, argv);
	Vtop* top = new Vtop{contextp};	//实例化模块
	//波形文件
 	Verilated::traceEverOn(true);			//启用波形跟踪
 	VerilatedFstC*tfp=new VerilatedFstC;		//创建跟踪对象
 	top->trace(tfp, 99);
 	tfp->open("waveform.fst");			//打开fst文件
 	int i=0;
	while (!contextp->gotFinish()){			//仿真循环
		int a = rand() & 1;
		int b = rand() & 1;
		top->a = a;
		top->b = b;
		top->eval();
		
		tfp->dump(i); 
		printf("a = %d, b = %d, f = %d\n", a, b, top->f);
		assert(top->f == (a ^ b));
		i++;
		}
 
	delete top;
	delete contextp;
	
	tfp->close();
	return 0;
}
