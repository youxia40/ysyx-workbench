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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

// this should be enough
static char buf[65536] = {};
static char code_buf[65536 + 128] = {}; // a little larger than `buf`
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = %s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";

// this should be enough
int choose(uint32_t n) {
  return rand() % n;
}

//len=strlen(buf)表示当前buf的实际长度（变值）
void gen(char c) {                                      //处理字符
  int len=strlen(buf);
  if (len >= sizeof(buf) - 2) {
    return;                                           // buf满,退出函数
  }
  buf[len] = c;                                           //依次填入buf
  buf[len + 1] = '\0';
}

void gen_rand_space() {
  int n = choose(4);                               // 0~3个空格
  int i;
  for (i = 0; i < n; i ++) {
    gen(' ');                                      //填入空格
  }
}

void gen_num() {
  int len = strlen(buf);
  if(len >= sizeof(buf) - 10) {
    return;                                       // buf满,退出函数
  }
  int num = rand() %1000;
  snprintf(buf+len,sizeof(buf)-len, "%d", num);   //数字转为字符串，填入buf
  gen_rand_space();
}

void gen_num_nozero() {
  int len = strlen(buf);
  if (len >= sizeof(buf) - 10) {
    return;                                       // buf满,退出函数
  }
  snprintf(buf+len, sizeof(buf)-len, "%d", rand() %999+ 1); //生成非零数
  if(rand()%4==0)
  {
    gen_rand_space();
  }
}

static char gen_rand_op() {
  const char ops[] = {'+', '-', '*', '/'}; //运算符
  char op = ops[rand() % 4];                //随机选择运算符
  gen(op);                //随机选择运算符

  if(rand()%4==0)
  {
    gen_rand_space();
  }
  return op;
}

static void gen_rand_expr(int depth) {
  if(strlen(buf)>20) {
    gen_num();                                   //填入数字
    return;
  }
  if (strlen(buf)>sizeof(buf)-10) {                               //深度限制
    gen_num();                                   //填入数字
    return;
  }
  if(strlen(buf) >= sizeof(buf) - 10) {
    gen_num_nozero();                     //填入数字
    return;                                       // buf满,退出函数
  }
  switch (choose(3)) {                      // 动态调整分支概率
    case 0: // 生成数字
      (rand()%5 == 0) ? gen_num_nozero() : gen_num();
      break;
  
    case 1: { // 括号表达式
      gen('(');
      gen_rand_expr(0);                           // 生成内部表达式
      gen(')');
      
      // 括号后必须接运算符
      if (strlen(buf) < sizeof(buf)-10) {
      char op=gen_rand_op();                                      // 随机运算符
      (op == '/') ? gen_num_nozero() : gen_rand_expr(0);                            // 生成数

      }
      break;
    }
    default: gen_rand_expr(0);                          //填入运算符
            char op=gen_rand_op();
            if(op=='/') {
              gen_num_nozero(); 
            }
            else {
              gen_rand_expr(0);
            }
            break;
  }
  return ;
}

int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);                        
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  for (i = 0; i < loop; i ++) {
    buf[0] = '\0';                                                    // reset buf
    gen_rand_expr(0);
    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc /tmp/.code.c -o /tmp/.expr");
    if (ret != 0) continue;

    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);

    int result;
    ret = fscanf(fp, "%d", &result);
    pclose(fp);

    printf("%u %s\n", result, buf);
  }
  return 0;
}
