#include "watchpoint.h"
#include "expr.h"
#include <assert.h>


typedef struct WP {//单个监视点对象,保存编号/表达式/上次值
  int id;
  bool used;
  char expr[256];
  uint32_t val;
} WP;

#define MAX_WP 32
static WP wp_pool[MAX_WP];//固定容量监视点池
static int next_id = 1;//单调递增编号,便于用户按ID删除监视点

//初始化监视点池
void wp_init() {
#if NPC_ENABLE_ASSERT
  assert(MAX_WP > 0);
#endif
  for (int i = 0; i < MAX_WP; i++) {
    wp_pool[i].id   = 0;//ID 0保留表示无效监视点
    wp_pool[i].used = false;
    wp_pool[i].expr[0] = '\0';//清空表达式字符串
    wp_pool[i].val  = 0;
  }
  next_id = 1;//重置ID计数器
}

//表达式先求值一次作为基准值
void wp_new(const char *expr) {
#if NPC_ENABLE_ASSERT
  assert(expr != NULL);
#endif
  if (!expr) {
    printf("watchpoint: missing expression\n");
    return;
  }

  int slot = -1; //可用下标
  for (int i = 0; i < MAX_WP; i++) {
    if (!wp_pool[i].used) {
      slot = i;//找到第一个空闲槽位
      break;
    }
  }

  if (slot < 0) {
    printf("watchpoint: no free slot (max %d)\n", MAX_WP);
    return;
  }

  bool ok = false;
  uint32_t v = expr_eval(expr, &ok);//求值表达式并检查合法性

  if (!ok) {
    printf("watchpoint: bad expr: %s\n", expr);
    return;
  }

  WP *wp = &wp_pool[slot];//初始化监视点对象
#if NPC_ENABLE_ASSERT
  assert(slot >= 0 && slot < MAX_WP);
#endif
  wp->id = next_id++;//分配唯一ID
  wp->used = true;//标记为已使用
  strncpy(wp->expr, expr, sizeof(wp->expr) - 1);//复制表达式字符串并确保末尾有'\0'
  wp->expr[sizeof(wp->expr) - 1] = '\0';//安全地终止字符串
  wp->val  = v;//保存初始值

  printf("watchpoint %d set: %s (0x%08X)\n", wp->id, wp->expr, wp->val);
}

//按ID删除监视点
void wp_del(int id) {
#if NPC_ENABLE_ASSERT
  assert(id >= 0);
#endif
  for (int i = 0; i < MAX_WP; i++) {
    if (wp_pool[i].used && wp_pool[i].id == id) {
      wp_pool[i].used = false;//标记为未使用
      //删除时，不清空表达式字符串和数值，因为它们不会被访问到，保留以供调试时查看

      printf("watchpoint %d deleted\n", id);
      return;
    }
  }
  printf("watchpoint %d not found\n", id);
}

//打印当前所有监视点
void wp_list() {
  printf("Num  Expr                         Value\n");
  printf("---- ---------------------------- ----------\n");
  for (int i = 0; i < MAX_WP; i++) {
    if (wp_pool[i].used) {
      printf("%-4d %-28s 0x%08X\n",wp_pool[i].id, wp_pool[i].expr, wp_pool[i].val);
    }
  }
}


//遍历全部监视点并比较新旧值，中则返回1，否则返回0
int wp_check() {//遍历全部监视点并比较新旧值
  bool hit = false;//是否有监视点命中

  for (int i = 0; i < MAX_WP; i++) {
    if (!wp_pool[i].used) {
      continue;
    }
  #if NPC_ENABLE_ASSERT
    assert(wp_pool[i].id > 0);
    assert(wp_pool[i].expr[0] != '\0');
  #endif

    bool ok = false;
    uint32_t cur = expr_eval(wp_pool[i].expr, &ok);//重新求值当前表达式并检查合法性
    if (!ok) {

      //表达式临时失效（如访问了无效寄存器名）不会触发停机，仅打印提示并保留监视点
      printf("watchpoint %d: expr invalid now: %s\n",wp_pool[i].id, wp_pool[i].expr);
      continue;
    }

    if (cur != wp_pool[i].val) {
      hit = true;
      printf("watchpoint %d triggered\n", wp_pool[i].id);
      printf("  expr: %s\n", wp_pool[i].expr);
      printf("  old : 0x%08X\n", wp_pool[i].val);
      printf("  new : 0x%08X\n", cur);

      //命中后把“新值”回填为下一轮比较基线，避免同一变化反复触发
      wp_pool[i].val = cur;//更新监视点的基准值
    }
  }

  return hit;
}
