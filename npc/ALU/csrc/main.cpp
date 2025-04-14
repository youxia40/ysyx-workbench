#include <nvboard.h>
#include <Vtop.h>
 
static TOP_NAME dut;
 
void nvboard_bind_all_pins(TOP_NAME* top);
void clk_clk()
{
	dut.clk=0;
	dut.eval();
	dut.clk=1;
	dut.eval();
	usleep(100);
}
int main() {
  nvboard_bind_all_pins(&dut);
  nvboard_init();
 
  while(1) {
    nvboard_update();
    clk_clk(); 
  }
}
