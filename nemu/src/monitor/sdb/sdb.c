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
  cpu_exec(-1);
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

  else {
    printf("Error!Invalid Argument!\n");
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

void sdb_set_batch_mode() {
  is_batch_mode = true;
}

void sdb_mainloop() {
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
  /* Compile the regular expressions. */
  init_regex();

  /* Initialize the watchpoint pool. */
  init_wp_pool();
}
