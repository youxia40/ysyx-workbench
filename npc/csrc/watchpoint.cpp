#include "watchpoint.h"
#include "expr.h"
#include <cstdio>
#include <cstring>

struct WP {
  int      id;
  bool     used;
  char     expr[256];
  uint32_t val;
};

static const int MAX_WP = 32;

static WP   wp_pool[MAX_WP];
static int  next_id = 1;

void wp_init() {
  for (int i = 0; i < MAX_WP; i++) {
    wp_pool[i].id   = 0;
    wp_pool[i].used = false;
    wp_pool[i].expr[0] = '\0';
    wp_pool[i].val  = 0;
  }
  next_id = 1;
}

void wp_new(const char *expr) {
  if (!expr) {
    printf("watchpoint: missing expression\n");
    return;
  }

  int slot = -1;
  for (int i = 0; i < MAX_WP; i++) {
    if (!wp_pool[i].used) {
      slot = i;
      break;
    }
  }

  if (slot < 0) {
    printf("watchpoint: no free slot (max %d)\n", MAX_WP);
    return;
  }

  bool ok = false;
  uint32_t v = expr_eval(expr, &ok);
  if (!ok) {
    printf("watchpoint: bad expr: %s\n", expr);
    return;
  }

  WP &wp = wp_pool[slot];
  wp.id   = next_id++;
  wp.used = true;
  std::strncpy(wp.expr, expr, sizeof(wp.expr) - 1);
  wp.expr[sizeof(wp.expr) - 1] = '\0';
  wp.val  = v;

  printf("watchpoint %d set: %s (0x%08X)\n", wp.id, wp.expr, wp.val);
}

void wp_del(int id) {
  for (int i = 0; i < MAX_WP; i++) {
    if (wp_pool[i].used && wp_pool[i].id == id) {
      wp_pool[i].used = false;
      printf("watchpoint %d deleted\n", id);
      return;
    }
  }
  printf("watchpoint %d not found\n", id);
}

void wp_list() {
  printf("Num  Expr                         Value\n");
  printf("---- ---------------------------- ----------\n");
  for (int i = 0; i < MAX_WP; i++) {
    if (wp_pool[i].used) {
      printf("%-4d %-28s 0x%08X\n",
             wp_pool[i].id, wp_pool[i].expr, wp_pool[i].val);
    }
  }
}

bool wp_check() {
  bool hit = false;

  for (int i = 0; i < MAX_WP; i++) {
    if (!wp_pool[i].used) continue;

    bool ok = false;
    uint32_t cur = expr_eval(wp_pool[i].expr, &ok);
    if (!ok) {
      printf("watchpoint %d: expr invalid now: %s\n",
             wp_pool[i].id, wp_pool[i].expr);
      continue;
    }

    if (cur != wp_pool[i].val) {
      hit = true;
      printf("watchpoint %d triggered\n", wp_pool[i].id);
      printf("  expr: %s\n", wp_pool[i].expr);
      printf("  old : 0x%08X\n", wp_pool[i].val);
      printf("  new : 0x%08X\n", cur);
      wp_pool[i].val = cur;
    }
  }

  return hit;
}
