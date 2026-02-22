#include "common.h"
#include "Vysyx_25040118_top.h"
#include "wave.h"
#include "sdb.h"
#include "itrace.h"
#include "mtrace.h"
#include "ftrace.h"
#include "difftest.h"
#include "loader.h"

#include <verilated.h>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <cassert>


extern NPCContext npc_ctx;

static void parse_args(int argc, char **argv,const char **image,bool *batch) {
  *image = nullptr;
  *batch = false;

  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-e") == 0 && i + 1 < argc) {//指定镜像路径参数
      *image = argv[++i];
    } else if (strcmp(argv[i], "-b") == 0) {//批处理模式参数(禁用SDB交互)
      *batch = true;
    } else {
      if (*image == nullptr) {//如果还有未解析的参数且镜像路径尚未设置,则把第一个非选项参数当作镜像路径
        *image = argv[i];
      }
    }
  }
}

// NPC主入口：完成初始化、主循环执行与退出判定
int main(int argc, char **argv) {
  const char *image = nullptr;//镜像路径(ELF或BIN)
  bool batch = false;//是否批处理模式(禁用SDB交互)
  parse_args(argc, argv, &image, &batch);//解析命令行参数
#if NPC_ENABLE_ASSERT
  assert(argc >= 1);
  assert(argv != nullptr);
#endif

  if (image == nullptr) {//没有指定镜像路径则打印用法并退出
    printf("[NPC] usage: %s [-e prog.elf|prog.bin] [-b]\n", argv[0]);
    return 1;
  }

  Verilated::commandArgs(argc, argv);//把命令行参数传给Verilator运行时
  Verilated::traceEverOn(NPC_ENABLE_WAVE ? true : false);//按开关决定是否启用波形trace

  std::memset(&npc_ctx, 0, sizeof(npc_ctx));//清零全局上下文,保证状态可重复


  //把common.h里面设置的宏开关进行赋值
  npc_ctx.debug.sdb_enabled = NPC_ENABLE_SDB && !batch;
  npc_ctx.debug.difftest_enabled = NPC_ENABLE_DIFFTEST;
  npc_ctx.debug.itrace_enabled = NPC_ENABLE_ITRACE;
  npc_ctx.debug.mtrace_enabled = NPC_ENABLE_MTRACE;
  npc_ctx.debug.ftrace_enabled = NPC_ENABLE_FTRACE;

  npc_load_image(&npc_ctx, image);//自动识别ELF/BIN并完成装载

  printf("[NPC] image loaded,entry=0x%08x\n", npc_ctx.entry);
  printf("[NPC] mem base=0x%08x,size=%dMB\n",
         MEM_BASE, MEM_SIZE / (1024*1024));

  auto *top = new Vysyx_25040118_top;//实例化顶层RTL模型
#if NPC_ENABLE_ASSERT
  assert(top != nullptr);
#endif
#if NPC_ENABLE_WAVE
  wave_init(top);//打开FST波形输出
#endif

#if NPC_ENABLE_ITRACE
  itrace_init(&npc_ctx);//初始化指令反汇编与环形缓冲
#endif
#if NPC_ENABLE_DIFFTEST
  difftest_init(&npc_ctx);//加载参考模型并同步初始状态
#endif
#if NPC_ENABLE_SDB
  sdb_init(&npc_ctx);//初始化命令调试器与监视点池
#endif
#if NPC_ENABLE_FTRACE
  ftrace_init(&npc_ctx);//加载ELF函数符号,用于call/ret打印
#endif

  printf("[NPC] sim start,max_cycles=%llu\n\n\n", (unsigned long long)MAX_CYCLES);

  int cyc = 0;//主循环计数器,每轮对应一个完整时钟周期(下降沿+上升沿)
  while (!npc_ctx.stop && cyc < MAX_CYCLES) {//周期上限保护,防止死循环


    top->clk = 0;//下降沿阶段
    top->rst = (cyc < 2) ? 1 : 0;//前两个周期保持复位
    top->eval();
  #if NPC_ENABLE_WAVE
    wave_dump(cyc * 2);//周期开始时的下降沿阶段记录波形
  #endif

    top->clk = 1;//上升沿阶段
    top->eval();
  #if NPC_ENABLE_WAVE
    wave_dump(cyc * 2 + 1);//周期结束时的上升沿阶段记录波形
  #endif

    bool dpi_stop = npc_ctx.stop;//DPI可能在本周期内提前置停
    //这里先缓存dpi_stop,是为了保证“本周期是否提交pc/inst”与后续trace判定一致
    //避免同一周期里stop状态变化导致提交信息和trace信息不一致

    if (!dpi_stop) {
      npc_ctx.pc   = top->pc_out;//提交当前PC
      npc_ctx.inst = top->inst_out;//提交当前指令
      //只在本周期未被DPI终止时提交,避免把trap后的无效输出当作最后一条提交指令
    }
    npc_ctx.cycles++;//全局周期计数
    npc_ctx.debug.cycle_count++;//调试周期计数


#if NPC_ENABLE_DIFFTEST
    if (!dpi_stop && !top->rst) {
      //difftest只在有效提交期执行,与DUT提交点严格对齐
      difftest_step(&npc_ctx);
    }
#endif

    if (top->stop) {
      npc_ctx.stop = true;//RTL死循环检测触发停止
      if (!npc_ctx.stop_reason) {
        npc_ctx.stop_reason = (char*)"top stop";
      }
      break;
    }

#if NPC_ENABLE_SDB


    if (!npc_ctx.debug.sdb_enabled) {//非交互态时先检查监视点
      sdb_wpchk(&npc_ctx);
    }

    //若sdb_enabled被置为true，则进入一次交互
    if (npc_ctx.debug.sdb_enabled) {//进入一次SDB命令处理
      sdb_step(&npc_ctx);
    }
#endif



    if (npc_ctx.inst == 0xFFFFFFFFu) {//如果提交了全1的非法指令,则增加非法计数,连续过多则保护性停止
      npc_ctx.debug.invalid_count++;
      if (npc_ctx.debug.invalid_count > 10) {
        npc_ctx.stop = true;//连续非法指令达到阈值后保护性停止
        npc_ctx.stop_reason = (char*)"invalid inst";//设置停止原因
      }
    } else {
      npc_ctx.debug.invalid_count = 0;
    }

    //GOOD TRAP/BAD TRAP标志由ebreak对应的DPI回调设置
    if (npc_ctx.debug.hit_good_trap) {
      npc_ctx.stop = true;
      npc_ctx.stop_reason = (char*)"GOOD TRAP";
    } else if (npc_ctx.debug.hit_bad_trap) {
      npc_ctx.stop = true;
      npc_ctx.stop_reason = (char*)"BAD TRAP";
    }

#if NPC_ENABLE_SDB
    //处理siN命令的剩余步数
    if (!npc_ctx.stop && npc_ctx.debug.sdb_step_count > 0) {
      npc_ctx.debug.sdb_step_count--;
      if (npc_ctx.debug.sdb_step_count == 0) {
        //si N执行完后重新进入交互,实现“执行N条后自动停在监视器”
        npc_ctx.debug.sdb_enabled = true;
      }
    }
#endif

    cyc++;
  }

  if (cyc >= MAX_CYCLES && !npc_ctx.stop) {
    npc_ctx.stop = true;//超周期上限时直接停止
    npc_ctx.stop_reason = (char*)"MAX_CYCLES";
    //这里标记为MAX_CYCLES而非PASS/FAIL,便于区分“功能失败”和“超时退出”
  }



  //要求GOOD TRAP发生时PC必须落在合法内存范围内
  bool pc_ok =
    (npc_ctx.pc >= MEM_BASE) &&
    (npc_ctx.pc <  MEM_BASE + MEM_SIZE);

  if (npc_ctx.debug.hit_good_trap && !pc_ok) {//如果GOOD TRAP发生但PC不合法,则警告并改判为BAD TRAP
    printf("[NPC][WARN] GOODTRAP at bad pc=0x%08x -> BADTRAP\n", npc_ctx.pc);
    npc_ctx.debug.hit_good_trap = false;
    npc_ctx.debug.hit_bad_trap  = true;
  }

  if (npc_ctx.debug.hit_good_trap) {//设置停止原因字符串
    npc_ctx.stop_reason = (char*)"GOOD TRAP";
  } else if (npc_ctx.debug.hit_bad_trap) {
    npc_ctx.stop_reason = (char*)"BAD TRAP";
  }

  printf("[NPC] cycles=%d\n", cyc);
  printf("[NPC] pc=0x%08x\n", npc_ctx.pc);
  printf("[NPC] reason=%s\n",npc_ctx.stop_reason ? npc_ctx.stop_reason : "(none)");

  if (npc_ctx.debug.hit_good_trap) {//如果是GOOD TRAP则输出返回码
    printf("[NPC] ret(a0)=%d (0x%08x)\n\n",
           (int32_t)npc_ctx.a0_value, npc_ctx.a0_value);
  }

#if NPC_ENABLE_WAVE
  wave_close();//关闭波形文件
#endif
  delete top;

  //通过条件:GOOD TRAP、返回码0、PC合法
  //把pc合法性也纳入pass条件,是为了防止“误触发GOODTRAP但已跑飞”的假阳性
  bool pass = npc_ctx.debug.hit_good_trap && (npc_ctx.a0_value == 0) && pc_ok;

#if NPC_ENABLE_ITRACE
  if (!pass && npc_ctx.debug.itrace_enabled) {//如果不通过且启用了指令追踪,则打印itrace诊断信息
    printf("\n--- itrace ---\n");
    itrace_dump_iringbuf();
  }
#endif

  return pass ? 0 : 1;
}
