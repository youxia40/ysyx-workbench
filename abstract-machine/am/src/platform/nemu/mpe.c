#include <am.h>
#include <stdatomic.h>
#include <klib-macros.h>

//MPE(Multi-Processor Extension) - 多处理器扩展, 为程序提供多处理器通信的能力 

bool mpe_init(void (*entry)()) {
  entry();
  panic("MPE entry returns");
}

int cpu_count() {
  return 1;
}

int cpu_current() {
  return 0;
}

int atomic_xchg(int *addr, int newval) {
  return atomic_exchange(addr, newval);
}
