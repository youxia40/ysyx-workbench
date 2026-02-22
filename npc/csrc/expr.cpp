#include "expr.h"
#include "common.h"
#include "dpi.h"
#include <cassert>


extern NPCContext npc_ctx;

typedef enum {
  TK_INVALID,//非法token
  TK_NUM,//数字常量
  TK_REG,//寄存器名
  TK_PLUS,//+
  TK_MINUS,//-
  TK_MUL,//*
  TK_DIV,///
  TK_EQ,//==
  TK_NEQ,//!=
  TK_LP,//(
  TK_RP,//)
  TK_AND,//&&
  TK_NEG,//一元-
  TK_DEREF//一元*
} TokenType;

typedef struct {
  TokenType type;//token类型
  char str[32];//原始文本
  uint32_t val;//数值缓存
} Token;

typedef struct {
  const char *pat;//匹配模式
  TokenType type;//命中后token类型
} Rule;

static Rule rules[] = {
  {" ",TK_INVALID},//空白分隔符
  {"+",TK_PLUS},
  {"-",TK_MINUS},
  {"*",TK_MUL},
  {"/",TK_DIV},
  {"(",TK_LP},
  {")",TK_RP},
  {"==",TK_EQ},
  {"!=",TK_NEQ},
  {"&&",TK_AND},
  {"0x",TK_NUM},//十六进制前缀
  {"$",TK_REG},//寄存器前缀
  {NULL,TK_INVALID}
};

//最多支持32个token
#define MAX_TOKENS 32
static Token toks[MAX_TOKENS];//token缓冲区
static int tok_cnt = 0;//当前token数量

static bool is_digit(char c) { return c >= '0' && c <= '9'; }//判断十进制字符
static bool is_hex(char c) { return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'); }//判断十六进制字符
static bool is_alpha(char c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '_'); }//判断标识符字符

//词法切分入口
static bool make_tokens(const char *e) {
#if NPC_ENABLE_ASSERT
  assert(e != NULL);
#endif
  tok_cnt = 0;//每次求值前清空token数量
  int i = 0;//输入串扫描指针
  while (e[i] != '\0' && tok_cnt < MAX_TOKENS) {
    if (e[i] == ' ') { i++; continue; }//跳过空格
    //尝试规则匹配
    int matched = 0;//当前字符是否成功匹配某条规则
    for (int r = 0; rules[r].pat != NULL; r++) {
      int len = strlen(rules[r].pat);//当前规则长度
      if (strncmp(e + i, rules[r].pat, len) == 0) {
        if (rules[r].type == TK_NUM && strncmp(e + i, "0x", 2) == 0) {
          //解析十六进制常量
          int j = i + 2;
          while (is_hex(e[j])) {
            j++;
          }
          int nlen = j - i;//数字字符串长度
          if (nlen > 2 && nlen < 18) {
            Token tk = {TK_NUM, {0}, 0};//构建数字token
            strncpy(tk.str, e + i, nlen);//提取数字字符串
            tk.str[nlen] = '\0';
            sscanf(tk.str, "%x", &tk.val);
            toks[tok_cnt++] = tk;
            i = j;
            matched = 1;
            break;
          }
        } else if (rules[r].type == TK_REG && e[i] == '$') {
          int j = i + 1;
          while (is_alpha(e[j]) || is_digit(e[j])) {
            j++;
          }
          int nlen = j - i;
          if (nlen > 1 && nlen < 32) {
            Token tk = {TK_REG, {0}, 0};//构建寄存器token
            strncpy(tk.str, e + i, nlen);
            tk.str[nlen] = '\0';
            toks[tok_cnt++] = tk;
            i = j;
            matched = 1;
            break;
          }
        } else if (rules[r].type == TK_NUM && is_digit(e[i])) {
          int j = i;
          while (is_digit(e[j])) {
            j++;
          }
          int nlen = j - i;
          if (nlen > 0 && nlen < 18) {
            Token tk = {TK_NUM, {0}, 0};//构建十进制数字token
            strncpy(tk.str, e + i, nlen);
            tk.str[nlen] = '\0';
            sscanf(tk.str, "%u", &tk.val);
            toks[tok_cnt++] = tk;
            i = j;
            matched = 1;
            break;
          }
        } else {
          Token tk = {rules[r].type, {0}, 0};//构建运算符token
          tk.str[0] = e[i];
          tk.str[1] = '\0';
          toks[tok_cnt++] = tk;
          i += len;
          matched = 1;
          break;
        }
      }
    }
    if (!matched) {
      return false;//任一位置无法匹配规则则整条表达式非法
    }
  }
  return true;
}

static void mark_unary() {//根据上下文把'*'/'-'改写成一元运算符
#if NPC_ENABLE_ASSERT
  assert(tok_cnt >= 0 && tok_cnt <= MAX_TOKENS);
#endif
  //根据前一个token类型判断当前'*'/'-'是二元还是一元运算符
  for (int i = 0; i < tok_cnt; i++) {
    if (toks[i].type == TK_MUL) {
      if (i == 0 ||
          (toks[i-1].type != TK_NUM &&
           toks[i-1].type != TK_REG &&
           toks[i-1].type != TK_RP)) {
        toks[i].type = TK_DEREF;//根据上下文把*改写成解引用
      }
    } else if (toks[i].type == TK_MINUS) {
      if (i == 0 ||
          (toks[i-1].type != TK_NUM &&
           toks[i-1].type != TK_REG &&
           toks[i-1].type != TK_RP)) {
        toks[i].type = TK_NEG;//根据上下文把-改写成一元负号
      }
    }
  }
}

static bool check_paren(int p, int q) {
#if NPC_ENABLE_ASSERT
  assert(p >= 0 && q >= 0 && p <= q && q < tok_cnt);
#endif
  //判断[p,q]是否由一对最外层括号完整包裹
  if (toks[p].type != TK_LP || toks[q].type != TK_RP) {
    return false;//两端必须是括号
  }
  int c = 0;//括号嵌套计数
  for (int i = p; i <= q; i++) {
    if (toks[i].type == TK_LP) {
      c++;
    }
    if (toks[i].type == TK_RP) {
      c--;
    }
    if (c < 0) {
      return false;
    }
    if (i != q && c == 0) {
      return false;
    }
  }
  return c == 0;
}

static int main_op(int p, int q) {
#if NPC_ENABLE_ASSERT
  assert(p >= 0 && q >= 0 && p <= q && q < tok_cnt);
#endif
  //在[p,q]里选“最外层且优先级最低”的主运算符
  int op = -1;//主运算符下标
  int best = 100;//当前最低优先级
  int c = 0;//括号深度
  for (int i = p; i <= q; i++) {
    if (toks[i].type == TK_LP) {
      c++;
      continue;
    }
    if (toks[i].type == TK_RP) {
      c--;
      continue;
    }
    if (c != 0) {
      continue;
    }
    int pri = 100;//当前运算符优先级
    switch (toks[i].type) {
      case TK_AND:   pri = 1; break;
      case TK_EQ:
      case TK_NEQ:   pri = 2; break;
      case TK_PLUS:
      case TK_MINUS: pri = 3; break;
      case TK_MUL:
      case TK_DIV:   pri = 4; break;
      case TK_NEG:
      case TK_DEREF: pri = 5; break;
      default:       continue;
    }
    if (pri <= best) {
      //同优先级时取更靠右的运算符,这样递归拆分可自然满足左结合二元运算的求值顺序
      best = pri;
      op   = i;
    }
  }
  return op;
}

static uint32_t eval(int p, int q, bool *s) {
#if NPC_ENABLE_ASSERT
  assert(s != NULL);
  assert(tok_cnt >= 0 && tok_cnt <= MAX_TOKENS);
#endif
  //递归求值token区间[p,q], *s表示本次求值是否成功
  if (p > q) {
    *s = false;
    return 0;
  }
  if (p == q) {
    //区间只剩一个token:只能是数字或寄存器
    if (toks[p].type == TK_NUM) {
      *s = true;
      return toks[p].val;
    }
    if (toks[p].type == TK_REG) {
      const char *name = toks[p].str + 1;//跳过$得到寄存器名
      if (strcmp(name, "pc") == 0) {
        *s = true;
        return npc_ctx.pc;
      }
      uint32_t regs[REG_NUM];//临时寄存器数组
      npc_get_regs(regs);
      for (int i = 0; i < REG_NUM; i++) {
        if (strcmp(name, reg_names[i]) == 0) {
          *s = true;
          return regs[i];
        }
      }
      *s = false;
      return 0;
    }
    *s = false;
    return 0;
  }
  if (check_paren(p, q)) {
    //去掉最外层括号后继续求值
    return eval(p + 1, q - 1, s);
  }
  int op = main_op(p, q);
  if (op == -1) {
    *s = false;
    return 0;
  }
  //一元运算
  if (toks[op].type == TK_NEG || toks[op].type == TK_DEREF) {
    //一元运算只消费右侧子表达式,因此递归区间是(op+1,q)
    uint32_t v = eval(op + 1, q, s);//递归求右操作数
    if (!*s) {
      return 0;
    }
    if (toks[op].type == TK_NEG) {
      return (uint32_t)(-(int32_t)v);
    } else {//解引用
      uint32_t vaddr = v;//虚拟地址
      //表达式求值场景下把小于MEM_BASE的地址视为0偏移,与当前DPI物理内存接口约定保持一致
      uint32_t paddr = (vaddr >= MEM_BASE) ? (vaddr - MEM_BASE) : 0;//转换为DPI期望的物理偏移
      return (uint32_t)npc_pmem_read((int)paddr);//通过DPI读取内存word
    }
  }
  //二元运算
  uint32_t v1 = eval(p, op - 1, s);//递归求左操作数
  if (!*s) {
    return 0;
  }
  uint32_t v2 = eval(op + 1, q, s);//递归求右操作数
  if (!*s) {
    return 0;
  }
  switch (toks[op].type) {
    case TK_PLUS:  return v1 + v2;
    case TK_MINUS: return v1 - v2;
    case TK_MUL:   return v1 * v2;
    case TK_DIV:
#if NPC_ENABLE_ASSERT
      assert(v2 != 0);
#endif
      return v1 / v2;
    case TK_EQ:    return v1 == v2;
    case TK_NEQ:   return v1 != v2;
    case TK_AND:   return (v1 && v2);
    default:
      *s = false;
      return 0;
  }
}

uint32_t expr_eval(const char *e, bool *success) {
  // 统一入口: 词法切分 -> 一元运算标注 -> 递归求值
#if NPC_ENABLE_ASSERT
  assert(e != NULL);
#endif
  bool local_ok = false;//success为空时使用的本地占位标志
  if (success == NULL) {
    success = &local_ok;
  }
  *success = make_tokens(e);//先做词法切分
  if (!*success) {
    return 0;
  }

  //mark_unary必须在词法切分之后、递归求值之前执行,否则无法区分二元*与一元*、二元-与一元-
  mark_unary();//重写一元负号与解引用
  return eval(0, tok_cnt - 1, success);//递归求值整个token区间
}
