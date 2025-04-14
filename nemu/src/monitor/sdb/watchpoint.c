/***************************************************************************************
* Copyright (c) 2014-2024 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include "sdb.h"
#include <cpu/cpu.h>
#define NR_WP 32


static WP wp_pool[NR_WP] = {};                      //监视点池(共32个watchpoint)
static WP *head = NULL, *free_ = NULL;                  //head用于组织使用中的监视点结构, free用于组织空闲的监视点结构

void init_wp_pool() {                             // init_wp_pool()函数会对两个链表进行初始化
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
  }

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */
WP* new_wp() {                                       //从free_链表中返回一个空闲的监视点结构
  WP *wp = free_;
  if (wp != NULL) {
    free_ = free_->next;                                //后移free_
    wp->next = head;                                     //向head头加入
    head = wp;                                        //wp存入head(更新head)
  }
  else {
    printf("No more available watchpoints !\n");
    return NULL;
  }
  return wp;
}

void free_wp(WP *wp) {                               //将wp归还到free_链表中
  if (wp == NULL) {
    printf("This watchpoint has been freed !\n");
    return;
  }

  if (wp ==head) {                                   //如果wp为head第一节点，直接更新
    head = wp->next;                               //将head指向wp下一个监视点
  }

  else {                                                    //处理的是非头节点
    WP *p = head;
    while (p != NULL && p->next!= wp) {                   //找到wp在head中的位置
      p = p->next;                                            //若p->next!=wp,则p后移
    }
    if (p != NULL) {
      p->next = wp->next;                            //将wp从head中删除
    }
  }
  wp->next =free_;                                     //将wp的next指针置空(从free取，则要放回到free_里)
  free_ = wp;                                        //将wp放回free_链表中
  printf("Watchpoint %d has been freed !\n", wp->NO); //输出监视点被释放
}


bool check_watchpoints() {
  if (head == NULL) {
    return false;                                 //无监视点时直接返回
  }

  WP *current = head;                               //遍历head链表
  bool triggered = false;
  
  while (current != NULL) {
    bool success = false;
    int new_value = expr(current->expr, &success);                                            //计算表达式
    
    if (!success) {                                                                                     //表达式计算失败处理
      printf("[Watchpoint Error] Invalid expression '%s' at wp%d\n",current->expr, current->NO); //自动删除无效监视点
      WP *next = current->next;                                                        //先保存下一个节点
      free_wp(current);                                                              //自动删除无效监视点
      current = next;                                                              //直接跳转到下一个节点
      continue;                                                                                    //跳过后续处理
      
    }
    else if (new_value != current->value) {                                                 //值变化处理
      if (!triggered) {                                             //首次触发时打印头信息
        printf("\n--- Watchpoint Triggered ---\n");
        triggered = true;
      }
      printf("Watchpoint #%d: %s\n", current->NO, current->expr);
      printf("Old value: 0x%08x\n", current->value);
      printf("New value: 0x%08x\n", new_value);
      current->value = new_value;                                                   //更新为最新值
    }
    
    current = current->next;
  }
  
  if (triggered) {
    printf("\n");
  }
  return triggered;                                                                //返回是否触发
}

void info_wp() {                                          //输出当前所有的监视点的变化情况
  WP *wp=head;    
  if (wp==NULL) {                                     //全为空，不用wp->if_used判断
    printf("No watchpoints set now!\n");
    return;
  }

  printf("NO\tExpr\tValue\t\n");
  while(wp!=NULL) {                                      //遍历head链表
    printf("%d\t%s\t0x%08x\n", wp->NO, wp->expr, wp->value);
    wp = wp->next;                                   //后移
  }
}

void delete_wp(int NO) {
  if (head == NULL) {
    printf("No watchpoints to delete!\n");
    return;
  }

  WP *current = head;                                     //遍历链表
  WP *p = NULL;                                      //p用于记录当前节点的前一个节点

  while (current != NULL) {
    if (current->NO == NO) {                                          //找到目标节点
      if (p == NULL) {
        head = current->next;                             //删除头节点
      } else {
        p->next = current->next;                                      //删除中间/尾部节点
      }
      free_wp(current);                                              //统一释放
      printf("Deleted watchpoint %d\n", NO);
      return;
    }

    p= current;
    current = current->next;
  }

  printf("Watchpoint %d not found!\n", NO);
}