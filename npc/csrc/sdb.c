#include "sdb.h"
#include "expr.h"
#include "watchpoint.h"
#include "dpi.h"
#include <assert.h>
#include <readline/readline.h>
#include <readline/history.h>



//打印SDB命令帮助
static void help() {
  printf("Commands:\n");
  printf("-  help      - show this message\n");
  printf("-  c         - continue\n");
  printf("-  q         - quit\n");
  printf("-  si [N]    - step N insns (default 1)\n");
  printf("-  info r    - show regs\n");
  printf("-  info w    - show watchpoints\n");
  printf("-  x N EXPR  - examine memory\n");
  printf("-  p EXPR    - eval expression\n");
  printf("-  w EXPR    - set watchpoint\n");
  printf("-  d N       - delete watchpoint N\n");
  printf("-  pt        - run expr tests\n");
}

//打印寄存器快照
void sdb_regs() {
  uint32_t regs[REG_NUM];//临时寄存器快照数组
#if NPC_ENABLE_ASSERT
  assert(REG_NUM > 0);
#endif
  npc_get_regs(regs);

  printf("Regs:\n");
  for (int i = 0; i < REG_NUM; i++) {
    printf("%4s(x%2d): 0x%08x  ", reg_names[i], i, regs[i]);
    if (i % 4 == 3) {
      printf("\n");
    }
  }
}

void sdb_x(int n, uint32_t addr) {
#if NPC_ENABLE_ASSERT
  assert(n > 0);
#endif
  //从给定虚拟地址开始,按word连续读取n项
  printf("Mem at 0x%08X:\n", addr);
  for (int i = 0; i < n; i++) {
    uint32_t vaddr = addr + i * 4;
    //expr里地址语义是"虚拟地址",而npc_pmem_read接口接收的是相对MEM_BASE的物理偏移
    uint32_t paddr = (vaddr >= MEM_BASE) ? (vaddr - MEM_BASE) : 0;
    uint32_t data  = npc_pmem_read((int)paddr);
    printf("0x%08X: 0x%08X\n", vaddr, data);
  }
}

//执行一次表达式求值并打印结果
static void eval_expr_cmd(const char *e) {
  bool ok = false;//表达式是否成功求值
  uint32_t v = expr_eval(e, &ok);
  if (ok) {
    printf("%s = %u (0x%08X)\n", e, v, v);
  } else {
    printf("bad expr: %s\n", e);
  }
}

//批量执行表达式测试文件并汇总通过率
static void expr_test() {
  const char *fn = "input";//测试样例文件(每行:期望值+表达式)
  FILE *fp = fopen(fn, "r");
  if (!fp) {
    printf("cannot open %s\n", fn);
    return;
  }

  char line[256]; //单行输入缓冲
  unsigned total = 0, pass = 0;//总用例数/通过数
  char buf[4096];
  size_t off = 0;//错误输出缓冲当前写入偏移
  buf[0] = '\0';

  while (fgets(line, sizeof(line), fp)) {
    total++;
    //去掉行尾换行，便于后续用sscanf按“参考值+表达式串”解析
    line[strcspn(line, "\r\n")] = '\0';

    uint32_t ref, val;//参考值与实际求值结果
    char expr[256];//被测表达式字符串
    if (sscanf(line, "%u %[^\n]", &ref, expr) != 2) {
      off += snprintf(buf + off, sizeof(buf) - off,"[%u] bad line: %s\n", total, line);//解析失败记录错误并继续,避免单行格式问题影响后续测试
      continue;
    }

    bool ok = false;//求值是否成功
    val = expr_eval(expr, &ok);//expr_eval内部会根据表达式语义访问寄存器/内存等,可能因为语法错误或越界访问等导致求值失败
    if (!ok) {
      off += snprintf(buf + off, sizeof(buf) - off,"[%u] invalid expr: %s\n", total, line);
      continue;
    }

    if (val != ref) {
      //此处只记录失败项并继续跑完全部样例，最后统一汇总
      off += snprintf(buf + off, sizeof(buf) - off,"[%u] wrong: %s\n  ref=%u (0x%08X)\n  got=%u (0x%08X)\n",total, expr, ref, ref, val, val);
      continue;
    }

    pass++;
  }

  fclose(fp);

  if (off > 0) {
    printf("Errors:\n%s", buf);
  }
  printf("Expr test: %u/%u ok\n", pass, total);
}



//继续
void sdb_cont(NPCContext *ctx) {
#if NPC_ENABLE_ASSERT
  assert(ctx != NULL);
#endif
  ctx->debug.sdb_enabled    = false;
  ctx->debug.sdb_step_count = 0;
  //continue语义是“退出交互并清空单步计数”，后续由主循环持续执行直到新的停止条件出现
  printf("continue\n");
}


//单步执行
void sdb_step(NPCContext *ctx) {
#if NPC_ENABLE_ASSERT
  assert(ctx != NULL);
#endif
  (void)system("stty sane 2>/dev/null");//进入SDB交互前恢复终端规范模式

  char *line = readline("(sdb) ");//readline负责行编辑和历史导航
  if (!line) {
    if (!ctx->stop && !ctx->debug.sdb_enabled) {
      (void)system("stty -icanon -echo min 0 time 0 2>/dev/null");
    }
    return;
  }

  char *input = line + strspn(line, " \t");
  if (!input || *input == '\0') {
    free(line);
    if (!ctx->stop && !ctx->debug.sdb_enabled) {
      (void)system("stty -icanon -echo min 0 time 0 2>/dev/null");
    }
    return;
  }

  HIST_ENTRY *last = previous_history();//获取上一条历史命令用于去重
  if (!last || !last->line || strcmp(last->line, input) != 0) {
    add_history(input);//仅当与上一条不同才写入历史，避免连续重复
  }
  if (last) {
    next_history();//恢复history游标，避免影响后续方向键导航
  }

  char *saveptr = NULL;
  char *cmd = strtok_r(input, " \t", &saveptr);//首个token作为命令字
  if (!cmd) {
    free(line);
    if (!ctx->stop && !ctx->debug.sdb_enabled) {
      (void)system("stty -icanon -echo min 0 time 0 2>/dev/null");
    }
    return;
  }

  if (strcmp(cmd, "help") == 0) {
    help();
  } else if (strcmp(cmd, "c") == 0) {
    sdb_cont(ctx);
  } else if (strcmp(cmd, "q") == 0) {
    ctx->stop = true;
    ctx->stop_reason = (char*)"user quit";//设置停止标志和原因，主循环会检测到并退出
    printf("quit\n");
  } else if (strcmp(cmd, "si") == 0) {
    char *nstr = strtok_r(NULL, " \t", &saveptr);
    int n = nstr ? atoi(nstr) : 1;//默认单步1条
    if (n <= 0) {
      printf("bad N: %s\n", nstr ? nstr : "(null)");//输入不合法时提示并保持交互状态不变
    } else {
      ctx->debug.sdb_step_count = n;//设置单步计数，主循环每执行一条指令就递减，计数归零时自动回到交互状态
      ctx->debug.sdb_enabled = false;
      //把sdb_enabled拉低后主循环会先连续执行N条，计数归零时再自动回到交互
      printf("step %d\n", n);
    }
  } else if (strcmp(cmd, "info") == 0) {
    char *sub = strtok_r(NULL, " \t", &saveptr);
    if (!sub) {
      printf("need subcmd: r/w\n");
    } else if (strcmp(sub, "r") == 0) {//info r命令显示寄存器状态
      sdb_regs();
    } else if (strcmp(sub, "w") == 0) {//info w命令显示当前所有监视点的状态
      wp_list();
    } else {
      printf("bad subcmd: %s\n", sub);
    }
  } else if (strcmp(cmd, "x") == 0) {//查看内存
    char *nstr = strtok_r(NULL, " \t", &saveptr);
    char *expr = saveptr ? (saveptr + strspn(saveptr, " \t")) : NULL;//余下整串作为表达式地址(允许内部带空格)
    if (!nstr || !expr || *expr == '\0') {
      printf("usage: x N EXPR\n");
    } else {
      int n = atoi(nstr);
      if (n <= 0) {
        printf("bad N: %s\n", nstr);
      } else {
        bool ok = false;
        uint32_t addr = expr_eval(expr, &ok);//求值表达式得到内存地址
        if (!ok) {
          printf("bad expr: %s\n", expr);
        } else {
          sdb_x(n, addr);//从求值得到的地址开始连续查看内存
        }
      }
    }
  } else if (strcmp(cmd, "p") == 0) {
    char *expr = saveptr ? (saveptr + strspn(saveptr, " \t")) : NULL;
    //p命令把剩余整行都交给表达式求值器，避免被空格截断
    if (!expr || *expr == '\0') {
      printf("usage: p EXPR\n");
    } else {
      eval_expr_cmd(expr);
    }
  } else if (strcmp(cmd, "w") == 0) {//设置监视点
    char *expr = saveptr ? (saveptr + strspn(saveptr, " \t")) : NULL;
    //监视点保存的是表达式字符串，后续每条指令后重算并比较新旧值
    if (!expr || *expr == '\0') {//w命令后必须跟表达式字符串，且允许内部带空格，因此直接把剩余整行当作表达式
      printf("usage: w EXPR\n");
    } else {
      wp_new(expr);
    }
  } else if (strcmp(cmd, "d") == 0) {//删除监视点
    char *idstr = strtok_r(NULL, " \t", &saveptr);
    if (!idstr) {
      printf("usage: d N\n");
    } else {
      int id = atoi(idstr);
      wp_del(id);
    }
  } else if (strcmp(cmd, "pt") == 0) {//运行表达式测试
    expr_test();
  } else {
    printf("unknown cmd: %s\n", cmd);
  }

  free(line);
  if (!ctx->stop && !ctx->debug.sdb_enabled) {
    (void)system("stty -icanon -echo min 0 time 0 2>/dev/null");//离开SDB恢复设备键盘轮询所需的非规范模式
  }

}




//初始化
void sdb_init(NPCContext *ctx) {
#if NPC_ENABLE_ASSERT
  assert(ctx != NULL);
#endif
  wp_init();//初始化监视点池
  printf("SDB ready . . .\n");
}

//每次检查监视点
void sdb_wpchk(NPCContext *ctx) {
#if NPC_ENABLE_ASSERT
  assert(ctx != NULL);
#endif
  if (wp_check()) {
    ctx->debug.sdb_step_count = 0;
    ctx->debug.sdb_enabled = true;
    //监视点命中后先打断运行，覆盖continue/si状态
    printf("watchpoint hit, enter sdb\n");
  }
}
