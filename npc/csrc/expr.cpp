#include "expr.h"
#include "common.h"
#include "dpi.h"
#include "mem.h"

#include <cctype>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <string>
#include <cstdio>
#include <regex>


extern NPCContext npc_ctx;

//运算符类型
enum TokenType {
  TK_INVALID,
  TK_NUM,
  TK_REG,
  TK_PLUS,                         // +
  TK_MINUS,                        // -
  TK_MUL,                         // *
  TK_DIV,                       // /
  TK_EQ,                         // ==
  TK_NEQ,                        // !=
  TK_LP,                       // (
  TK_RP,                       // )
  TK_AND,                      // &&
  TK_NEG,                      // 一元-
  TK_DEREF                      // 一元*
};

struct Token {
  TokenType   type;
  std::string str;
  uint32_t    val;
};

struct Rule {
  std::string pat;
  TokenType   type;
};

static Rule rules[] = {
  {"\\s+",              TK_INVALID},          //空白
  {"\\+",               TK_PLUS},
  {"-",                 TK_MINUS},
  {"\\*",               TK_MUL},
  {"/",                 TK_DIV},
  {"\\(",               TK_LP},
  {"\\)",               TK_RP},
  {"==",                TK_EQ},
  {"!=",                TK_NEQ},
  {"&&",                TK_AND},
  {"0[xX][0-9a-fA-F]+", TK_NUM},
  {"[0-9]+",            TK_NUM},
  {"\\$[a-zA-Z0-9_]+",  TK_REG},
};

static std::vector<Token> toks;

//分析
static bool make_tokens(const char *e) {
  toks.clear();
  std::string s(e);
  size_t p = 0;

  while (p < s.size()) {
    bool ok = false;
    for (auto &r : rules) {
      std::regex re(r.pat);
      std::smatch m;
      std::string sub = s.substr(p);
      if (std::regex_search(sub, m, re,
            std::regex_constants::match_continuous)) {
        std::string t = m.str();
        p += t.size();
        if (r.type != TK_INVALID) {
          Token tk;
          tk.type = r.type;
          tk.str  = t;
          if (r.type == TK_NUM) {
            if (t.size() > 2 &&
                t[0]=='0' && (t[1]=='x' || t[1]=='X')) {
              tk.val = std::stoul(t.substr(2), nullptr, 16);
            } else {
              tk.val = std::stoul(t, nullptr, 10);
            }
          }
          toks.push_back(tk);
        }
        ok = true;
        break;
      }
    }
    if (!ok) return false;
  }
  return true;
}


static void mark_unary() {
  for (size_t i = 0; i < toks.size(); i++) {
    auto &t = toks[i];
    if (t.type == TK_MUL) {
      if (i == 0 ||
          (toks[i-1].type != TK_NUM &&
           toks[i-1].type != TK_REG &&
           toks[i-1].type != TK_RP)) {
        t.type = TK_DEREF;
      }
    } else if (t.type == TK_MINUS) {
      if (i == 0 ||
          (toks[i-1].type != TK_NUM &&
           toks[i-1].type != TK_REG &&
           toks[i-1].type != TK_RP)) {
        t.type = TK_NEG;
      }
    }
  }
}

static bool check_paren(int p, int q) {
  if (toks[p].type != TK_LP || toks[q].type != TK_RP) return false;
  int c = 0;
  for (int i = p; i <= q; i++) {
    if (toks[i].type == TK_LP) c++;
    if (toks[i].type == TK_RP) c--;
    if (c < 0) return false;
    if (i != q && c == 0) return false;
  }
  return c == 0;
}

//查找主运算符（最外层、优先级最低）
static int main_op(int p, int q) {
  int op = -1;
  int best = 100;
  int c = 0;

  for (int i = p; i <= q; i++) {
    if (toks[i].type == TK_LP) { c++; continue; }
    if (toks[i].type == TK_RP) { c--; continue; }
    if (c != 0) continue;

    int pri = 100;
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
      best = pri;
      op   = i;
    }
  }
  return op;
}

static uint32_t eval(int p, int q, bool *s) {
  if (p > q) {
    *s = false;
    return 0;
  }

  if (p == q) {
    if (toks[p].type == TK_NUM) {
      *s = true;
      return toks[p].val;
    }
    if (toks[p].type == TK_REG) {
      std::string name = toks[p].str.substr(1);

      //特判 pc
      if (name == "pc") {
        *s = true;
        return npc_ctx.pc;
      }

      uint32_t regs[REG_NUM];
      npc_get_regs(regs);
      for (int i = 0; i < REG_NUM; i++) {
        if (name == reg_names[i]) {
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
    return eval(p + 1, q - 1, s);
  }

  int op = main_op(p, q);
  if (op == -1) {
    *s = false;
    return 0;
  }

  // 一元运算
  if (toks[op].type == TK_NEG || toks[op].type == TK_DEREF) {
    uint32_t v = eval(op + 1, q, s);
    if (!*s) return 0;

    if (toks[op].type == TK_NEG) {
      return (uint32_t)(-(int32_t)v);
    } else { // 解引用
      uint32_t vaddr = v;
      uint32_t paddr = (vaddr >= MEM_BASE) ? (vaddr - MEM_BASE) : 0;
      return (uint32_t)npc_pmem_read((int)paddr);
    }
  }

  //二元运算
  uint32_t v1 = eval(p, op - 1, s);
  if (!*s) return 0;
  uint32_t v2 = eval(op + 1, q, s);
  if (!*s) return 0;

  switch (toks[op].type) {
    case TK_PLUS:  return v1 + v2;
    case TK_MINUS: return v1 - v2;
    case TK_MUL:   return v1 * v2;
    case TK_DIV:   return v1 / v2;
    case TK_EQ:    return v1 == v2;
    case TK_NEQ:   return v1 != v2;
    case TK_AND:   return (v1 && v2);
    default:
      *s = false;
      return 0;
  }
}

uint32_t expr_eval(const char *e, bool *success) {
  bool local_ok = false;
  if (success == nullptr) {
    success = &local_ok;
  }

  *success = make_tokens(e);
  if (!*success) return 0;

  mark_unary();

  return eval(0, (int)toks.size() - 1, success);
}
