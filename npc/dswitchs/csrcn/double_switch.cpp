#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include"Vtop.h"
#include"verilated_fst_c.h"
#include"verilated.h"

VerilatedContext* contextp = NULL;
VerilatedFstC* tfp = NULL;

int main(int argc, char** argv){
	//激励文件
	contextp = new VerilatedContext;
	tfp=new VerilatedFstC;		//创建跟踪对象                    
	Vtop* top = new Vtop{contextp};	//实例化模块
	//波形文件
 	
 	contextp->traceEverOn(true);
 	top->trace(tfp, 99);
 	tfp->open("waveform.fst");			//打开fst文件
 	int i=0;
	while (i<100){			//仿真循环
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
 	tfp->close();
	delete top;
	delete contextp;
	return 0;
}
