#include "common.h"
#include "dpi.h"
#include "Vysyx_25040118_top.h"
#include "wave.h"
#include "sdb.h"
#include "itrace.h"
#include "mtrace.h"
#include "ftrace.h"
#include "difftest.h"
#include "loader.h"
#include "mem.h"

#include <verilated.h>
#include <verilated_fst_c.h>
#include <cstdlib>
#include <cstring>
#include <cstdio>


extern NPCContext npc_ctx;

#if NPC_ENABLE_ITRACE
extern void itrace_dump_iringbuf(void);
#endif

static void parse_args(int argc, char **argv,
                       const char **elf,
                       const char **log_file,
                       bool *batch) {
  *elf      = nullptr;
  *log_file = nullptr;
  *batch    = false;

  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-e") == 0 && i + 1 < argc) {
      *elf = argv[++i];
    } else if (strcmp(argv[i], "-l") == 0 && i + 1 < argc) {
      *log_file = argv[++i];
    } else if (strcmp(argv[i], "-b") == 0) {
      *batch = true;
    } else {
      if (*elf == nullptr) {
        *elf = argv[i];
      }
    }
  }
}

int main(int argc, char **argv) {
  const char *elf      = nullptr;
  const char *log_file = nullptr;
  bool batch = false;
  parse_args(argc, argv, &elf, &log_file, &batch);

  if (elf == nullptr) {
    printf("Usage: %s [-e prog.elf] [-l log] [-b]\n", argv[0]);
    return 1;
  }

  Verilated::commandArgs(argc, argv);
  Verilated::traceEverOn(true);

  std::memset(&npc_ctx, 0, sizeof(npc_ctx));

  npc_ctx.debug.sdb_enabled      = NPC_ENABLE_SDB && !batch;
  npc_ctx.debug.difftest_enabled = NPC_ENABLE_DIFFTEST;
  npc_ctx.debug.itrace_enabled   = NPC_ENABLE_ITRACE;
  npc_ctx.debug.mtrace_enabled   = NPC_ENABLE_MTRACE;
  npc_ctx.debug.ftrace_enabled   = NPC_ENABLE_FTRACE;

  npc_load_elf(&npc_ctx, elf);

  printf("ELF loaded, entry=0x%08x\n", npc_ctx.entry);
  printf("MEM: base=0x%08x size=%dMB\n",
         MEM_BASE, MEM_SIZE / (1024*1024));

  auto *top = new Vysyx_25040118_top;
  wave_init(top);

#if NPC_ENABLE_ITRACE
  itrace_init(&npc_ctx);
#endif
#if NPC_ENABLE_DIFFTEST
  difftest_init(&npc_ctx);
#endif
#if NPC_ENABLE_SDB
  sdb_init(&npc_ctx);
#endif
#if NPC_ENABLE_FTRACE
  ftrace_init(&npc_ctx);
#endif

  printf("Sim start, max %d cycles\n", MAX_CYCLES);

  int cyc = 0;
  while (!npc_ctx.stop && cyc < MAX_CYCLES) {


    top->clk = 0;
    top->rst = (cyc < 2) ? 1 : 0;
    top->eval();
    wave_dump(cyc * 2);


    top->clk = 1;
    top->eval();
    wave_dump(cyc * 2 + 1);

    bool dpi_stop = npc_ctx.stop;

    if (!dpi_stop) {
      npc_ctx.pc   = top->pc_out;
      npc_ctx.inst = top->inst_out;
    }
    npc_ctx.cycles++;
    npc_ctx.debug.cycle_count++;

#if NPC_ENABLE_ITRACE
    if (!dpi_stop && !top->rst) {
      itrace_step(&npc_ctx);
    }
#endif
#if NPC_ENABLE_MTRACE
    if (!dpi_stop) {
      mtrace_step(&npc_ctx);
    }
#endif
#if NPC_ENABLE_FTRACE
    if (!dpi_stop) {
      ftrace_step(&npc_ctx);
    }
#endif
#if NPC_ENABLE_DIFFTEST
    if (!dpi_stop && !top->rst) {
      difftest_step(&npc_ctx);
    }
#endif

    if (top->stop) {
      npc_ctx.stop = true;
      if (!npc_ctx.stop_reason) {
        npc_ctx.stop_reason = (char*)"top stop";
      }
      break;
    }

#if NPC_ENABLE_SDB


    if (!npc_ctx.debug.sdb_enabled) {
      sdb_wpchk(&npc_ctx);
    }

    //若sdb_enabled被置为true，则进入一次交互
    if (npc_ctx.debug.sdb_enabled) {
      sdb_step(&npc_ctx);
    }
#endif



    if (npc_ctx.inst == 0xFFFFFFFFu) {
      npc_ctx.debug.invalid_count++;
      if (npc_ctx.debug.invalid_count > 10) {
        npc_ctx.stop = true;
        npc_ctx.stop_reason = (char*)"invalid inst";
      }
    } else {
      npc_ctx.debug.invalid_count = 0;
    }

    //GOOD/BAD TRAP标志由ebreak的DPI设置
    if (npc_ctx.debug.hit_good_trap) {
      npc_ctx.stop = true;
      npc_ctx.stop_reason = (char*)"GOOD TRAP";
    } else if (npc_ctx.debug.hit_bad_trap) {
      npc_ctx.stop = true;
      npc_ctx.stop_reason = (char*)"BAD TRAP";
    }

#if NPC_ENABLE_SDB


    //处理si N
    if (!npc_ctx.stop && npc_ctx.debug.sdb_step_count > 0) {
      npc_ctx.debug.sdb_step_count--;
      if (npc_ctx.debug.sdb_step_count == 0) {
        npc_ctx.debug.sdb_enabled = true;
      }
    }
#endif

    cyc++;
  }

  if (cyc >= MAX_CYCLES && !npc_ctx.stop) {
    npc_ctx.stop = true;
    npc_ctx.stop_reason = (char*)"MAX_CYCLES";
  }



  //令GOOD TRAP的PC在合法范围
  bool pc_ok =
    (npc_ctx.pc >= MEM_BASE) &&
    (npc_ctx.pc <  MEM_BASE + MEM_SIZE);

  if (npc_ctx.debug.hit_good_trap && !pc_ok) {
    printf("WARN: GOOD TRAP at bad PC=0x%08x -> BAD\n", npc_ctx.pc);
    npc_ctx.debug.hit_good_trap = false;
    npc_ctx.debug.hit_bad_trap  = true;
  }

  if (npc_ctx.debug.hit_good_trap) {
    npc_ctx.stop_reason = (char*)"GOOD TRAP";
  } else if (npc_ctx.debug.hit_bad_trap) {
    npc_ctx.stop_reason = (char*)"BAD TRAP";
  }

  printf("\n=== END ===\n");
  printf("cycles  : %d\n", cyc);
  printf("pc      : 0x%08x\n", npc_ctx.pc);
  printf("reason  : %s\n",
         npc_ctx.stop_reason ? npc_ctx.stop_reason : "(none)");

  if (npc_ctx.debug.hit_good_trap) {
    printf("ret(a0): %d (0x%08x)\n",
           (int32_t)npc_ctx.a0_value, npc_ctx.a0_value);
  }

  wave_close();
  delete top;

  bool pass =
    npc_ctx.debug.hit_good_trap &&
    (npc_ctx.a0_value == 0) &&
    pc_ok;

#if NPC_ENABLE_ITRACE
  if (!pass && npc_ctx.debug.itrace_enabled) {
    printf("\n--- iringbuf ---\n");
    itrace_dump_iringbuf();
  }
#endif

  return pass ? 0 : 1;
}
