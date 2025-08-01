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
#include <memory/paddr.h>
#include <common.h>
#include <elf.h>

void init_rand();
void init_log(const char *log_file);
void init_mem();
void init_difftest(char *ref_so_file, long img_size, int port);
void init_device();
void init_sdb();
void init_disasm();

static void welcome() {
  Log("Trace: %s", MUXDEF(CONFIG_TRACE, ANSI_FMT("ON", ANSI_FG_GREEN), ANSI_FMT("OFF", ANSI_FG_RED)));
  IFDEF(CONFIG_TRACE, Log("If trace is enabled, a log file will be generated "
        "to record the trace. This may lead to a large log file. "
        "If it is not necessary, you can disable it in menuconfig"));
  Log("Build time: %s, %s", __TIME__, __DATE__);
  printf("Welcome to %s-NEMU!\n", ANSI_FMT(str(__GUEST_ISA__), ANSI_FG_YELLOW ANSI_BG_RED));
  printf("For help, type \"help\"\n");
  
}

#ifndef CONFIG_TARGET_AM
#include <getopt.h>

void sdb_set_batch_mode();

static char *log_file = NULL;
static char *diff_so_file = NULL;
static char *img_file = NULL;
static int difftest_port = 1234;

static long load_img() {                                                          //加载镜像文件，镜像文件是一个二进制文件，包含了程序的代码和数据
  if (img_file == NULL) {
    Log("No image is given. Use the default build-in image.");
    return 4096; // built-in image size
  }

  FILE *fp = fopen(img_file, "rb");
  Assert(fp, "Can not open '%s'", img_file);

  fseek(fp, 0, SEEK_END);
  long size = ftell(fp);

  Log("The image is %s, size = %ld", img_file, size);

  fseek(fp, 0, SEEK_SET);
  int ret = fread(guest_to_host(RESET_VECTOR), size, 1, fp);                      //将镜像文件加载到内存中，RESET_VECTOR是程序的入口地址
  assert(ret == 1);

  fclose(fp);
  return size;
}


char *elf_file = NULL;                                       //ELF文件路径
void parse_elf(const char *elf_file);                            //声明解析elf文件的函数，位于elf.c文件


static int parse_args(int argc, char *argv[]) {                     //解析命令行参数
  const struct option table[] = {
    {"batch"    , no_argument      , NULL, 'b'},                    //设置批处理模式
    {"log"      , required_argument, NULL, 'l'},                    //设置日志文件
    {"diff"     , required_argument, NULL, 'd'},                    //设置差异测试的so文件
    {"port"     , required_argument, NULL, 'p'},                    //设置差异测试的端口号
    {"help"     , no_argument      , NULL, 'h'},
    {"elf"      , required_argument, NULL, 'e'},                    //设置elf文件
    {0          , 0                , NULL,  0 },                    //结束标志
  };
  int o;
  while ( (o = getopt_long(argc, argv, "-bhl:d:p:e:", table, NULL)) != -1) {
    switch (o) {
      case 'e': elf_file = optarg; break;                         //设置elf文件
      case 'b': sdb_set_batch_mode(); break;                        //设置批处理模式
      case 'p': sscanf(optarg, "%d", &difftest_port); break;              //设置端口号
      case 'l': log_file = optarg; break;                                   //设置日志文件
      case 'd': diff_so_file = optarg; break;                                     //设置差异测试的so文件
      case 1: img_file = optarg; return 0;                                //设置镜像文件
      default:
        printf("Usage: %s [OPTION...] IMAGE [args]\n\n", argv[0]);
        printf("\t-b,--batch              run with batch mode\n");
        printf("\t-l,--log=FILE           output log to FILE\n");
        printf("\t-d,--diff=REF_SO        run DiffTest with reference REF_SO\n");
        printf("\t-p,--port=PORT          run DiffTest with port PORT\n");
        printf("\t-e,--elf=FILE           parse the elf file\n");
        printf("\n");
        exit(0);
    }
  }
  return 0;
}



void init_monitor(int argc, char *argv[]) {                                       //初始化monitor
  /* Perform some global initialization. */

  /* Parse arguments. */
  parse_args(argc, argv);                                     //解析命令行参数，位于monitor.c

  /* Parse the ELF file if provided. */
  if (elf_file) {                                                         //用于解析ELF文件
    parse_elf(elf_file); 
  } else {
    printf("No ELF file provided, ftrace disabled\n");
  }
  
  /* Set random seed. */
  init_rand();

  /* Open the log file. */
  init_log(log_file); 

  /* Initialize memory. */
  init_mem();                                               //生成内存（位于paddr.c文件）

  /* Initialize devices. */
  IFDEF(CONFIG_DEVICE, init_device());                          //初始化设备，位于device.c文件

  /* Perform ISA dependent initialization. */
  init_isa();                                                    //初始化ISA                     

  /* Load the image to memory. This will overwrite the built-in image. */
  long img_size = load_img();                                           //指定加载镜像文件，位于monitor.c

  /* Initialize differential testing. */
  init_difftest(diff_so_file, img_size, difftest_port);

  /* Initialize the simple debugger. */
  init_sdb();

  IFDEF(CONFIG_ITRACE, init_disasm());                          //初始化反汇编器，位于disasm.c文件

  /* Display welcome message. */
  welcome();
}
#else // CONFIG_TARGET_AM
static long load_img() {
  extern char bin_start, bin_end;
  size_t size = &bin_end - &bin_start;
  Log("img size = %ld", size);
  memcpy(guest_to_host(RESET_VECTOR), &bin_start, size);
  return size;
}

void am_init_monitor() {
  init_rand();
  init_mem();
  init_isa();
  load_img();
  IFDEF(CONFIG_DEVICE, init_device());
  welcome();
}
#endif
