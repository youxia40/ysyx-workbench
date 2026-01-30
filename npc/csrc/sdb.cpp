#include "sdb.h"
#include "expr.h"
#include "watchpoint.h"
#include "dpi.h"
#include "mem.h"

#include <readline/readline.h>
#include <readline/history.h>
#include <cstring>
#include <cstdlib>
#include <cstdio>



static void help() {
  printf("Commands:\n");
  printf("  help       - show this message\n");
  printf("  c          - continue\n");
  printf("  q          - quit\n");
  printf("  si [N]     - step N insns (default 1)\n");
  printf("  info r     - show regs\n");
  printf("  info w     - show watchpoints\n");
  printf("  x N EXPR   - examine memory\n");
  printf("  p EXPR     - eval expression\n");
  printf("  w EXPR     - set watchpoint\n");
  printf("  d N        - delete watchpoint N\n");
  printf("  pt         - run expr tests\n");
}

void sdb_regs() {
  uint32_t regs[REG_NUM];
  npc_get_regs(regs);

  printf("Regs:\n");
  for (int i = 0; i < REG_NUM; i++) {
    printf("%-4s(x%2d): 0x%08x", reg_names[i], i, regs[i]);
    if (i % 4 == 3) printf("\n");
  }
}

void sdb_x(int n, uint32_t addr) {
  printf("Mem at 0x%08X:\n", addr);
  for (int i = 0; i < n; i++) {
    uint32_t vaddr = addr + i * 4;
    uint32_t paddr = (vaddr >= MEM_BASE) ? (vaddr - MEM_BASE) : 0;
    uint32_t data  = npc_pmem_read((int)paddr);
    printf("0x%08X: 0x%08X\n", vaddr, data);
  }
}

static void eval_expr_cmd(const char *e) {
  bool ok = false;
  uint32_t v = expr_eval(e, &ok);
  if (ok) {
    printf("%s = %u (0x%08X)\n", e, v, v);
  } else {
    printf("bad expr: %s\n", e);
  }
}

static void expr_test() {
  const char *fn = "input";
  FILE *fp = fopen(fn, "r");
  if (!fp) {
    printf("cannot open %s\n", fn);
    return;
  }

  char line[256];
  unsigned total = 0, pass = 0;
  char buf[4096];
  size_t off = 0;
  buf[0] = '\0';

  while (fgets(line, sizeof(line), fp)) {
    total++;
    line[strcspn(line, "\r\n")] = '\0';

    uint32_t ref, val;
    char expr[256];
    if (sscanf(line, "%u %[^\n]", &ref, expr) != 2) {
      off += snprintf(buf + off, sizeof(buf) - off,
                      "[%u] bad line: %s\n", total, line);
      continue;
    }

    bool ok = false;
    val = expr_eval(expr, &ok);
    if (!ok) {
      off += snprintf(buf + off, sizeof(buf) - off,
                      "[%u] invalid expr: %s\n", total, line);
      continue;
    }

    if (val != ref) {
      off += snprintf(buf + off, sizeof(buf) - off,
                      "[%u] wrong: %s\n  ref=%u (0x%08X)\n  got=%u (0x%08X)\n",
                      total, expr, ref, ref, val, val);
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
  ctx->debug.sdb_enabled    = false;
  ctx->debug.sdb_step_count = 0;
  printf("continue\n");
}

//单步执行
void sdb_step(NPCContext *ctx) {
  char *line = readline("(sdb) ");
  if (!line) return;
  if (*line) add_history(line);

  char *cmd = std::strtok(line, " ");
  if (!cmd) {
    free(line);
    return;
  }

  if (strcmp(cmd, "help") == 0) {
    help();
  } else if (strcmp(cmd, "c") == 0) {
    sdb_cont(ctx);
  } else if (strcmp(cmd, "q") == 0) {
    ctx->stop = true;
    ctx->stop_reason = (char*)"user quit";
    printf("quit\n");
  } else if (strcmp(cmd, "si") == 0) {
    char *nstr = std::strtok(nullptr, " ");
    int n = nstr ? atoi(nstr) : 1;
    if (n <= 0) {
      printf("bad N: %s\n", nstr ? nstr : "(null)");
    } else {
      ctx->debug.sdb_step_count = n;
      ctx->debug.sdb_enabled    = false;
      printf("step %d\n", n);
    }
  } else if (strcmp(cmd, "info") == 0) {
    char *sub = std::strtok(nullptr, " ");
    if (!sub) {
      printf("need subcmd: r/w\n");
    } else if (strcmp(sub, "r") == 0) {
      sdb_regs();
    } else if (strcmp(sub, "w") == 0) {
      wp_list();
    } else {
      printf("bad subcmd: %s\n", sub);
    }
  } else if (strcmp(cmd, "x") == 0) {
    char *nstr = std::strtok(nullptr, " ");
    char *expr = std::strtok(nullptr, "");
    if (!nstr || !expr) {
      printf("usage: x N EXPR\n");
    } else {
      int n = atoi(nstr);
      if (n <= 0) {
        printf("bad N: %s\n", nstr);
      } else {
        bool ok = false;
        uint32_t addr = expr_eval(expr, &ok);
        if (!ok) {
          printf("bad expr: %s\n", expr);
        } else {
          sdb_x(n, addr);
        }
      }
    }
  } else if (strcmp(cmd, "p") == 0) {
    char *expr = std::strtok(nullptr, "");
    if (!expr) {
      printf("usage: p EXPR\n");
    } else {
      eval_expr_cmd(expr);
    }
  } else if (strcmp(cmd, "w") == 0) {
    char *expr = std::strtok(nullptr, "");
    if (!expr) {
      printf("usage: w EXPR\n");
    } else {
      wp_new(expr);
    }
  } else if (strcmp(cmd, "d") == 0) {
    char *idstr = std::strtok(nullptr, " ");
    if (!idstr) {
      printf("usage: d N\n");
    } else {
      int id = atoi(idstr);
      wp_del(id);
    }
  } else if (strcmp(cmd, "pt") == 0) {
    expr_test();
  } else {
    printf("unknown cmd: %s\n", cmd);
  }

  free(line);
}




//初始化
void sdb_init(NPCContext *ctx) {
  (void)ctx;
  wp_init();
  printf("SDB ready. Type 'help'.\n");
}

//每次检查监视点
void sdb_wpchk(NPCContext *ctx) {
  if (wp_check()) {
    ctx->debug.sdb_step_count = 0;
    ctx->debug.sdb_enabled    = true;
    printf("watchpoint hit, enter sdb\n");
  }
}
