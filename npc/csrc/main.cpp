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
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <cstdint>
#include <dlfcn.h>
#include <readline/readline.h>
#include <readline/history.h>

// 使用全局上下文
extern NPCContext npc_ctx;

#if NPC_ENABLE_ITRACE
// 出错时打印 iringbuf 的接口（在 itrace.cpp 里实现）
extern void itrace_dump_iringbuf(void);
#endif

// 命令行参数解析
static void parse_args(int argc, char** argv,
                       const char** elf_file,
                       const char** log_file,
                       bool* batch_mode) {
  *elf_file   = nullptr;
  *log_file   = nullptr;
  *batch_mode = false;

  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-e") == 0 && i + 1 < argc) {
      *elf_file = argv[++i];
    } else if (strcmp(argv[i], "-l") == 0 && i + 1 < argc) {
      *log_file = argv[++i];
    } else if (strcmp(argv[i], "-b") == 0) {
      *batch_mode = true;
    } else {
      // 未显式指定 -e 时，将第一个非选项参数视为 ELF 路径
      if (*elf_file == nullptr) {
        *elf_file = argv[i];
      }
    }
  }
}

int main(int argc, char** argv) {
  // 解析命令行参数
  const char* elf_file = nullptr;
  const char* log_file = nullptr;
  bool batch_mode      = false;
  parse_args(argc, argv, &elf_file, &log_file, &batch_mode);

  if (elf_file == nullptr) {
    printf("Usage: %s [-e <program.elf>] [-l <log_file>] [-b]\n", argv[0]);
    return 1;
  }

  // 初始化 Verilator
  Verilated::commandArgs(argc, argv);
  Verilated::traceEverOn(true);

  // 重置全局上下文
  memset(&npc_ctx, 0, sizeof(npc_ctx));

  // 设置调试开关
  npc_ctx.debug.sdb_enabled      = NPC_ENABLE_SDB   && !batch_mode;
  npc_ctx.debug.difftest_enabled = NPC_ENABLE_DIFFTEST;
  npc_ctx.debug.itrace_enabled   = NPC_ENABLE_ITRACE;
  npc_ctx.debug.mtrace_enabled   = NPC_ENABLE_MTRACE;
  npc_ctx.debug.ftrace_enabled   = NPC_ENABLE_FTRACE;

  // 加载程序
  npc_load_elf(&npc_ctx, elf_file);

  printf("ELF loaded successfully. Entry point: 0x%08x\n", npc_ctx.entry);
  printf("Memory base: 0x%08x, Memory size: %d MB\n",
         MEM_BASE, MEM_SIZE / (1024 * 1024));

  // 创建顶层模块实例
  Vysyx_25040118_top* top = new Vysyx_25040118_top;

  // 初始化波形
  wave_init(top);

  // 初始化基础设施
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

  printf("Starting simulation with max %d cycles...\n", MAX_CYCLES);
  printf("Press Ctrl+C to stop simulation early\n");

  // 主循环
  int cycle = 0;
  while (!npc_ctx.stop && cycle < MAX_CYCLES) {
    // 低电平阶段
    top->clk = 0;
    top->rst = (cycle < 2) ? 1 : 0; // 前两拍保持复位
    top->eval();
    wave_dump(cycle * 2);

    // 上升沿
    top->clk = 1;
    top->eval();
    wave_dump(cycle * 2 + 1);

    // 这一拍是否已经被 DPI (ebreak 等) 置 stop
    bool dpi_stopped = npc_ctx.stop;

    if (!dpi_stopped) {
      npc_ctx.pc   = top->pc_out;
      npc_ctx.inst = top->inst_out;
    }
    npc_ctx.cycles++;
    npc_ctx.debug.cycle_count++;

#if NPC_ENABLE_ITRACE
    // 只在复位结束后才记录 itrace，避免 _start 第一条指令打印两遍
    if (!dpi_stopped && !top->rst) {
      itrace_step(&npc_ctx);
    }
#endif

#if NPC_ENABLE_MTRACE
    if (!dpi_stopped) {
      mtrace_step(&npc_ctx);
    }
#endif

#if NPC_ENABLE_FTRACE
    if (!dpi_stopped) {
      ftrace_step(&npc_ctx);
    }
#endif

#if NPC_ENABLE_DIFFTEST
    // 只有在复位结束之后(!top->rst)才开始 DiffTest 对拍
    if (!dpi_stopped && !top->rst) {
      difftest_step(&npc_ctx);
    }
#endif

    // 顶层 stop 信号（死循环检测等）
    if (top->stop) {
      npc_ctx.stop = true;
      if (npc_ctx.stop_reason == nullptr) {
        npc_ctx.stop_reason = (char*)"NPC stop signal";
      }
      break;
    }

#if NPC_ENABLE_SDB
    // 单步调试
    if (npc_ctx.debug.sdb_enabled) {
      sdb_step(&npc_ctx);
    }
#endif

    // 监视点检查
    if (npc_ctx.debug.watchpoint_count > 0) {
      sdb_check_watchpoints(&npc_ctx);
    }

    // 简单非法指令保护：同一条非法指令连续多次出现则终止
    if (npc_ctx.inst == 0xFFFFFFFFu) {
      npc_ctx.debug.invalid_count++;
      if (npc_ctx.debug.invalid_count > 10) {
        npc_ctx.stop = true;
        npc_ctx.stop_reason = (char*)"Invalid instruction";
      }
    } else {
      npc_ctx.debug.invalid_count = 0;
    }

    // 检查 GOOD/BAD TRAP（由 npc_ebreak 设置 hit_good_trap / hit_bad_trap）
    if (npc_ctx.debug.hit_good_trap) {
      npc_ctx.stop = true;
      npc_ctx.stop_reason = (char*)"GOOD TRAP";
    } else if (npc_ctx.debug.hit_bad_trap) {
      npc_ctx.stop = true;
      npc_ctx.stop_reason = (char*)"BAD TRAP";
    }

    cycle++;
  }

  if (cycle >= MAX_CYCLES && !npc_ctx.stop) {
    npc_ctx.stop_reason = (char*)"MAX_CYCLES reached";
  }

  // ======= 在这里做一次“GOOD TRAP 合法性检查” =======
  // 要求 GOOD TRAP 的 PC 在 NPC 内存范围内
  bool pc_in_mem =
      (npc_ctx.pc >= MEM_BASE) &&
      (npc_ctx.pc <  MEM_BASE + MEM_SIZE);

  if (npc_ctx.debug.hit_good_trap && !pc_in_mem) {
    printf("Warning: GOOD TRAP at invalid PC=0x%08x, "
           "treating it as BAD TRAP!\n",
           npc_ctx.pc);
    npc_ctx.debug.hit_good_trap = false;
    npc_ctx.debug.hit_bad_trap  = true;
  }

  // 最终根据标志修正 stop_reason（GOOD TRAP 优先）
  if (npc_ctx.debug.hit_good_trap) {
    npc_ctx.stop_reason = (char*)"GOOD TRAP";
  } else if (npc_ctx.debug.hit_bad_trap) {
    npc_ctx.stop_reason = (char*)"BAD TRAP";
  }

  printf("\n=== Simulation Summary ===\n");
  printf("Cycles     : %d\n", cycle);
  printf("Final PC   : 0x%08x\n", npc_ctx.pc);
  printf("Exit Reason: %s\n",
         npc_ctx.stop_reason ? npc_ctx.stop_reason : "(none)");

  if (npc_ctx.debug.hit_good_trap) {
    printf("Return Code (a0): %d (0x%08x)\n",
           (int32_t)npc_ctx.a0_value, npc_ctx.a0_value);
  }

  wave_close();
  delete top;

  // 只有：到达 GOOD TRAP 且 a0 == 0 且 PC 合法，才视为 PASS，返回 0
  bool pass =
      npc_ctx.debug.hit_good_trap &&
      (npc_ctx.a0_value == 0) &&
      pc_in_mem;

#if NPC_ENABLE_ITRACE
  // 如果本次运行没通过，而且开启了 itrace，则把 iringbuf 输出出来
  if (!pass && npc_ctx.debug.itrace_enabled) {
    printf("\n=== ITrace: recent instructions (iringbuf) ===\n");
    itrace_dump_iringbuf();
  }
#endif

  return pass ? 0 : 1;
}