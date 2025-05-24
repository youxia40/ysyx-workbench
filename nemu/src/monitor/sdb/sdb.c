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

#include <isa.h>
#include <cpu/cpu.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "sdb.h"
#include <memory/paddr.h> 
#include <stdbool.h>
#include <stdio.h>

static int is_batch_mode = false;

void init_regex();
void init_wp_pool();

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);                                           //切记：-1转换无符号整数的最大值！因此此命令非单步执行
  return 0;
}


static int cmd_q(char *args) {
	nemu_state.state = NEMU_QUIT;
  return -1;
}

static int cmd_help(char *args);

static int cmd_si(char *args){
  int N =1;                              //N为执行参数次数
  if(args != NULL) {
    N = atoi(args);                      //atoi函数是标准库函数，用于将字符串转换为整数
    if (N <= 0) {
      printf("Error!Invalid Argument!\n");
      return 0;
    }
  }
  else {
    N=1;
  }
  cpu_exec(N);
  return 0;
}

static int cmd_info(char *args) {                  
  if (args == NULL) {
    printf("Error!Invalid Argument!\n");
    return 0;
  }

  if (strcmp(args,"r")==0){
    isa_reg_display();
  }
  else if (strcmp(args , "w")==0) {
    info_wp();
  }
  else {
    printf("Error!The Invalid Argument!\n");
  }
  return 0;
}

static int cmd_x(char *args) {
  if (args == NULL) {
    printf("Error!Please input the number and the address!\n");
    return 0;
  }

  char *arg = strtok(args, " ");
  if (arg == NULL) {
    printf("Error!Invalid Argument!\n");
    return 0;
  }

  int N = atoi(arg);                              //n为读取内存的字节数
  if (N<= 0) {
    printf("Error!Invalid Argument!\n");
    return 0;
  }

  arg = strtok(NULL, " ");              //现在arg为读取内存的地址
  if (arg == NULL) {
    printf("Error!Please input the address!\n");
    return 0;
  }

  paddr_t address = strtol(arg, NULL, 16);           //将arg转换为十六进制地址

  for(int i=0;i<N;i++) {
    printf("0x%08x: ",address+i*4);
      for(int j=0;j<4;j++) {
        printf("0x%08x ",paddr_read(address+i*4+j,4));//读取内存地址
      }
      printf("\n");
  }

  return 0;
}

static int cmd_p(char *args) {
  if (args == NULL) {
    printf("Error!Invalid Argument!Please input your expression!\n");
    return 0;
  }
  //表达式计算
  bool success = false;                                     //eval中会设为true,不用管
  int result = expr(args, &success);      
  if (!success) {
    printf("Error!Invalid Expression!\n");
    return 0;
  }
  printf("DNUM: %d    HNUM: 0x%08x\n",result,result); 
  return 0;
}

/* 表达式测试*/
static int cmd_pt(char *args) {
  char path[256];                                                               //存储inpu文件的路径
  snprintf(path, sizeof(path), "%s/tools/gen-expr/input", getenv("NEMU_HOME"));        //getenv函数获取环境变量NEMU_HOME的值
  FILE *fp = fopen(path, "r");
  if(fp==NULL) {
    printf("Error!File %s not found!\n",path);
    return 0;
  }

  char raw_line[65536];                               //读取行的缓冲区
  char error_log[16384] = {0};                      //存储错误信息的缓冲区
  unsigned error_offset = 0;                  //存储错误信息的下标（当前写入位置）
  unsigned total = 0,pass = 0;

  while(fgets(raw_line, sizeof(raw_line), fp) != NULL) {                //从文件中读取一行
    total++;
    raw_line[strcspn(raw_line, "\r\n")] = '\0';     //去掉换行符

    uint32_t expected, real;
    bool success = false;
    char expr_buf[65536];                           //表达式的缓冲区

    // 解析错误
    if (sscanf(raw_line, "%u %[^\r\n]", &expected, expr_buf) != 2) {
      error_offset += snprintf(error_log + error_offset, sizeof(error_log) - error_offset,"[Line %04u] Bad Format: %s\n",total, raw_line);
      continue;
    }

     // 表达式错误
    real = expr(expr_buf, &success);
    if (!success) {
      error_offset += snprintf(error_log + error_offset,sizeof(error_log) - error_offset,"[Line %04u] Invalid Expression: %s\n",total, raw_line);
      continue;
    }

    // 结果错误
    if (expected != real) {
      error_offset += snprintf(error_log + error_offset,sizeof(error_log) - error_offset,
        "[Line %04u] Mismatch: %s\n"
        "  Expected: %u (0x%08x)\n"
        "  Actual  : %u (0x%08x)\n",total, raw_line, expected, expected, real, real);
      continue;
    }
    pass++;
  }

  fclose(fp);

  if (error_offset > 0) {
    printf("\nError Report:\n%s", error_log);                //输出错误信息
  }
  printf("\nTest finished: %d/%d passed\n", pass, total);   //输出测试结果
  
  return 0;
}

static int cmd_w(char *args){
 if (args == NULL) {
    printf("Error!Blank Argument!\n");
    return 0;
  }

  bool success;
  int value = expr(args, &success);
  if (!success) {
    printf("The expression is problematic!\n");
    return 0;
  }
  else {
    WP *wp = new_wp();
    
    if (strlen(args) >= 64) {                                     //表达式长度限制
      printf("Error: Expression exceeds max length (%d)\n", 64-1);
      free_wp(wp);                                                            // 立即释放无效的监视点
      return 0;
    }
    wp->value = value;
    
    strcpy(wp->expr, args);
    printf("Watchpoint %d: %s\n", wp->NO, wp->expr);
    printf("Value: 0x%08x\n", wp->value);
  }
  return 0;
}

static int cmd_d(char *args){
  if (args == NULL) {
    printf("Error!Blank Argument!\n");
    return 0;
  }

  int n = atoi(args);
  if (n <= 0) {
    printf("Error!Invalid Argument!\n");
    return 0;
  }

  delete_wp(n);
  return 0;
}

static struct {
  const char *name;
  const char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display information about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "si", "Execute one instruction", cmd_si },
  { "info", "Show information about registers or watchpoints", cmd_info },
  { "x", "Examine memory", cmd_x },
  { "p", "Evaluate expression", cmd_p },
  { "pt", "Test expression", cmd_pt },
  { "w", "Set watchpoint", cmd_w },
  { "d", "Delete watchpoint", cmd_d },
  /* TODO: Add more commands */

};

#define NR_CMD ARRLEN(cmd_table)

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void sdb_set_batch_mode() {                                   //设置批处理模式
  is_batch_mode = true;
}

void sdb_mainloop() {                                            //nemu交互（sdb主循环）
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef CONFIG_DEVICE
    extern void sdl_clear_event_queue();
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}

void init_sdb() {
  /* Compile the regular expressions. 编译正则表达式*/
  init_regex();
                   //测试表达式
  /* Initialize the watchpoint pool. 初始化监视点（池）*/
  init_wp_pool();                             
}
