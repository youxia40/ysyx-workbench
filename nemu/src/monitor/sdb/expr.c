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
#include "common.h"
#include <assert.h>
#include <isa.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>
#include <stdbool.h>
#include <sys/types.h>
#include <memory/paddr.h>

enum {
  TK_NOTYPE = 256, TK_EQ=257,TK_NEQ=258,
  TK_DNUM=259,TK_HNUM=260,TK_NEG=261, //TK_NEG负号        
  TK_TEQ=262,//解引用
  TK_REG=263, //寄存器
  TK_AND=264, //and
  /* TODO: Add more token types */
};

static struct rule {
  const char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */
  {" +", TK_NOTYPE},    // spaces空格
  {"\\+", '+'},         // plus
  {"\\-", '-'},         // minus or 负
  {"\\*", '*'},         // multiply or 解引用
  {"\\/", '/'},         // divide
  {"\\(", '('},         // left parenthesis
  {"\\)", ')'},         // right parenthesis
  {"==", TK_EQ},        // equal
  {"!=", TK_NEQ},       // not equal
  {"&&", TK_AND},     //and
  {"0[xX][0-9a-fA-F]+", TK_HNUM}, // hexadecimal number十六进制
  {"[0-9]+", TK_DNUM},      // decimal number十进制
  {"\\$[a-zA-Z0-9_]+", TK_REG},   //寄存器
};

#define NR_REGEX ARRLEN(rules)

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {             //用于进行rule规则的编译
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}
 
typedef struct token {
  int type;
  char str[32];
} Token;

static Token tokens[65536] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;         //已识别token的数量

//词法分析
static bool make_token(char *e) {                   //处理token
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;                                         //已识别token的数量(归0)

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {//regexec函数执行正则表达式匹配
        char *substr_start = e + position;                          //子串起始位置
        int substr_len = pmatch.rm_eo;                              //子串长度

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);

        position += substr_len;                               //更新位置,向后移动

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {                          //读取到的token类型
          case TK_NOTYPE: break;
          case '+':
          case '-':
          case '*':
          case '/':
          case '(':
          case ')':
          case TK_AND:
          case TK_REG:
          case TK_EQ:
          case TK_NEQ:
          case TK_DNUM:
          case TK_HNUM: {
            tokens[nr_token].type = rules[i].token_type;            //类型
            strncpy(tokens[nr_token].str, substr_start, substr_len);//将子串复制到tokens的str中
            tokens[nr_token].str[substr_len] = '\0';              //字符串结束符(数组的最后一位是len的值)
            nr_token++;                                           //roken数量加1
            break;
          }
          default: {
            printf("we don't support this token type now,the token type is %d\n", rules[i].token_type);
            assert(0);                                    //出现未记录的规则 
            
          }
        }
        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

//递归求值
static bool check_parentheses(int p, int q) {                 //匹配括号(对子表达式)
  if(tokens[p].type != '(' || tokens[q].type != ')') {
    return false;
  }

  int i;
  int count = 0;
  for (i = p; i <= q; i++) {
    switch(tokens[i].type) {
      case '(':count++;break;
      case ')':count--;break;
      default: break;
    }
    if (i!=q && count == 0) {
      return false;                              //确保左右两端括号匹配
    }
    if (count<0) {
      return false;                               //出现"())"的情况
    }
}
  return count==0;                               //确保左右两端括号匹配
}

bool if_op(int type) {                      //判断是否是运算符
  return type == '+' || type == '-' || type == '*' || type == '/' || 
         type == TK_EQ || type == TK_NEQ;
}

int find_op(int p,int q) {                               //查找主运算符
  int i;
  int op=-1;
  int skip_parentheses=0;                                 //用于跳过括号内的运算符
  int op_priority=-1;                                     //主运算符的优先级
  for (i = q; i>=p; i--) {
    if(tokens[i].type == ')') {
      skip_parentheses++;
      continue;         
    }
    else if (tokens[i].type == '(') {
      skip_parentheses--;
      if(skip_parentheses<0) {                         //左括号不匹配
        return -1;
      }
      continue;
    }
    if(skip_parentheses!=0) {                            //跳过括号内的运算符
      continue;
    }

    bool is_unary=false;                          //单目运算符
    if (tokens[i].type == '-' || tokens[i].type == '*') {
        is_unary = (i == p) || (i > p && (tokens[i-1].type == '(' || if_op(tokens[i-1].type)));
    }
    else {
        is_unary=false;
    }
    if (is_unary) {
      if (tokens[i].type == '-') {
        tokens[i].type = TK_NEG;                               //将负号转换为TK_NEG
      }
      else if (tokens[i].type == '*') {
        tokens[i].type = TK_TEQ;                          //将解引用转换为TK_TEQ
      }
    }

    int curr_priority =-1;                                //运算符的优先级
    if (is_unary) {                               //单目运算符
      curr_priority = 1;
    }
    else {                                        //双目运算符
      curr_priority = 0;
    }     
    switch(tokens[i].type) {                              //数字越小，优先级越高
      case TK_NEG:
      case TK_TEQ: curr_priority = 1;break;                //负号or解引用
      case '*':curr_priority=is_unary ? 1 : 3;break;        // 解引用or乘法
      case '-':curr_priority=is_unary ? 1 : 4;break;        // 负号or减法
      case '/':curr_priority=3;break;
      case '+':curr_priority=4;break;
      case TK_EQ: 
      case TK_NEQ: curr_priority = 5;break;
      case TK_AND: curr_priority = 6;break;
      default:curr_priority=-1;break;
    }

    //选择优先级最低且最右的运算符
    if (curr_priority !=-1 && (curr_priority > op_priority || ((curr_priority == op_priority) && i>op))) {  //跳过数字
      op_priority = curr_priority;
      op = i;
   }
  }
  return op;
}

int eval(int p,int q,bool*success) {                                        //求值函数的框架
  *success=true;
  int op;
  int value0,value1, value2;                              //value0计算单目,value1与value2计算双目
  op=p;                                                     //主运算符的位置

  if (p > q) {
    /* Bad expression */
    *success=false;
    printf("Bad expression!(empty subexpression)\n");
    return 0;
  }

  else if (p == q) {
    /* Single token.
     * For now this token should be a number.
     * Return the value of the number.返回数字的值
     */
    if (tokens[p].type == TK_DNUM) {
      *success=true;
      return strtol(tokens[p].str, NULL, 10);                //十进制
    }
    else if (tokens[p].type == TK_HNUM) {
      *success=true;
      return strtol(tokens[p].str+2, NULL, 16);                    //十六进制(前缀0x要去掉)
    }
    else if (tokens[p].type == TK_EQ) {
      *success=false;
      printf("Error! \"==\" cannot exist on its own!\n");
      return 0;
    }
    else if (tokens[p].type == TK_NEQ) {
      *success=false;
      printf("Error! \"!=\" cannot exist on its own!\n");
      return 0;
    }
    else if (tokens[p].type == TK_NEG) {
      *success=false;
      printf("Error! \"-\" cannot exist on its own!\n");
      return 0;
    }
    else if (tokens[p].type == TK_TEQ) {
      *success=false;
      printf("Error! \"*\" cannot exist on its own!\n");
      return 0;
    }
    else if (tokens[p].type == TK_AND) {
      *success=false;
      printf("Error! \"&&\" cannot exist on its own!\n");
      return 0;
    }

    else if(tokens[p].type == TK_REG) {
      bool reg_success;
      int reg_num = isa_reg_str2val(tokens[p].str,&reg_success);          //将寄存器名转换为寄存器值
      if (!success) {
        printf("Undefined register: %s\n", tokens[p].str);
        *success = false;
        return 0;
      }
      *success = true;
      return reg_num;
    }
  }

  else if (check_parentheses(p, q) == true) {
    /* The expression is surrounded by a matched pair of parentheses.
     * If that is the case, just throw away the parentheses. 
     */
     if(!*success) {
      return 0;
      }
    return eval(p + 1, q - 1,success);                                      //去括号
  }

  else {                                  //计算表达式(无括号)
    /* We should do more things here. */
    op =find_op(p,q);                                                   /*the position of 主运算符 in the token expression;*/
    if (op==-1) {
      *success = false;
      printf("No main operator found between %d -- %d\n", p, q);
      return 0;
    }

    if (op==p) {                              //计算单目运算符
      value0=eval(op+1,q,success);
      if(!*success) {
        return 0;
      }
      switch (tokens[op].type) {
        case TK_NEG: return -value0;          //负号
        case TK_TEQ:              
        if(value0 % 4==0){
          *success=true;
          return paddr_read(value0,4); //解引用
        }
        else {
          *success=false;
          printf("Error!Unaligned memory access at 0x%08x\n",value0);
          return 0;
        }
        default:*success=false;
                printf("We don't have such a unary operator: %d\n",tokens[op].type);
                return 0;
      }
    }

    //计算双目运算符
    value1 = eval(p, op-1,success);                     
    if (!*success){                               //两if的作用：阻断错误的传递
      return 0;
    } 
    value2 = eval(op+1, q,success);
    if (!*success){
      return 0;
    }

    switch (tokens[op].type) {
      case '+': return value1 + value2;
      case '-': return value1 - value2;
      case '*': return value1 * value2;
      case '/':
      if (value2 == 0) {
        *success = true;
        printf("You cannot use 0 as a divisor!!\n");
        return 0;
      }
      return value1 / value2;
      case TK_EQ: return value1 == value2;
      case TK_NEQ: return value1 != value2;
      case TK_DNUM: return strtol(tokens[op].str, NULL, 10);        //十进制
      case TK_HNUM: return strtol(tokens[op].str, NULL, 16);          //十六进制
      case TK_AND: return value1 && value2;
      default:*success=false;
              printf("Unknown binary operator: %d\n", tokens[op].type);
              return 0;
    }
  }
  return 0;
}

word_t expr(char *e, bool *success) {         //表达式计算
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
    return eval(0, nr_token-1,success);         //返回表达式的值
}
