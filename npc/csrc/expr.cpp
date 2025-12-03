#include "expr.h"
#include "common.h"
#include "dpi.h"
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <map>
#include <string>
#include <cstddef>
#include <cstdio>
#include <regex>
#include <memory>

// 令牌类型
enum TokenType {
    TK_INVALID,
    TK_NUMBER,
    TK_REGISTER,
    TK_PLUS,        // +
    TK_MINUS,       // -
    TK_MUL,         // *
    TK_DIV,         // /
    TK_EQ,          // ==
    TK_NEQ,         // !=
    TK_LPAREN,      // (
    TK_RPAREN,      // )
    TK_AND,         // &&
    TK_NEG,         // 负号
    TK_DEREF        // 解引用 *
};

// 令牌结构
struct Token {
    TokenType type;
    std::string text;
    uint32_t value;
};

// 规则结构
struct Rule {
    std::string regex;
    TokenType token_type;
};

// 规则列表
static Rule rules[] = {
    {"\\s+", TK_INVALID},   // 空格
    {"\\+", TK_PLUS},
    {"-", TK_MINUS},
    {"\\*", TK_MUL},
    {"/", TK_DIV},
    {"\\(", TK_LPAREN},
    {"\\)", TK_RPAREN},
    {"==", TK_EQ},
    {"!=", TK_NEQ},
    {"&&", TK_AND},
    {"0[xX][0-9a-fA-F]+", TK_NUMBER}, // 十六进制
    {"[0-9]+", TK_NUMBER},            // 十进制
    {"\\$[a-zA-Z0-9_]+", TK_REGISTER}, // 寄存器
};

// 令牌列表
static std::vector<Token> tokens;

// 词法分析
static bool tokenize(const char* expr) {
    tokens.clear();
    std::string input(expr);
    size_t pos = 0;

    while (pos < input.size()) {
        bool matched = false;
        for (const auto& rule : rules) {
            std::regex re(rule.regex);
            std::smatch match;
            std::string substr = input.substr(pos);
            if (std::regex_search(substr, match, re, std::regex_constants::match_continuous)) {
                std::string token_str = match.str();
                pos += token_str.size();

                if (rule.token_type != TK_INVALID) {
                    Token token;
                    token.type = rule.token_type;
                    token.text = token_str;

                    // 处理数字
                    if (rule.token_type == TK_NUMBER) {
                        if (token_str.size() > 2 && (token_str[0] == '0' && (token_str[1] == 'x' || token_str[1] == 'X'))) {
                            token.value = std::stoul(token_str.substr(2), nullptr, 16);
                        } else {
                            token.value = std::stoul(token_str, nullptr, 10);
                        }
                    }
                    tokens.push_back(token);
                }
                matched = true;
                break;
            }
        }
        if (!matched) {
            // 无法匹配任何规则
            return false;
        }
    }
    return true;
}

// 检查括号匹配
static bool check_parentheses(int p, int q) {
    if (tokens[p].type != TK_LPAREN || tokens[q].type != TK_RPAREN) {
        return false;
    }

    int count = 0;
    for (int i = p; i <= q; i++) {
        if (tokens[i].type == TK_LPAREN) count++;
        if (tokens[i].type == TK_RPAREN) count--;
        if (count < 0) return false;
        if (i != q && count == 0) return false;
    }
    return count == 0;
}

// 查找主运算符
static int find_main_op(int p, int q) {
    int main_op = -1;
    int min_priority = 100; // 优先级数值越小，优先级越高
    int count = 0; // 括号计数

    for (int i = p; i <= q; i++) {
        if (tokens[i].type == TK_LPAREN) count++;
        if (tokens[i].type == TK_RPAREN) count--;
        if (count != 0) continue;

        int priority = 100;
        switch (tokens[i].type) {
            case TK_DEREF: priority = 1; break;
            case TK_NEG:   priority = 1; break;
            case TK_MUL:   priority = 2; break;
            case TK_DIV:   priority = 2; break;
            case TK_PLUS:  priority = 3; break;
            case TK_MINUS: priority = 3; break;
            case TK_EQ:    priority = 4; break;
            case TK_NEQ:   priority = 4; break;
            case TK_AND:   priority = 5; break;
            default:       priority = 100;
        }

        if (priority < min_priority) {
            min_priority = priority;
            main_op = i;
        }
    }
    return main_op;
}

// 递归求值
static uint32_t eval(int p, int q, bool* success) {
    if (p > q) {
        *success = false;
        return 0;
    }

    if (p == q) {
        switch (tokens[p].type) {
            case TK_NUMBER:
                *success = true;
                return tokens[p].value;
            case TK_REGISTER: {
                // 提取寄存器名称
                std::string reg_name = tokens[p].text.substr(1); // 移除$
                for (int i = 0; i < REG_NUM; i++) {
                    if (reg_name == reg_names[i]) {
                        uint32_t regs[REG_NUM];
                        npc_get_regs(regs);
                        *success = true;
                        return regs[i];
                    }
                }
                *success = false;
                return 0;
            }
            default:
                *success = false;
                return 0;
        }
    }

    if (check_parentheses(p, q)) {
        return eval(p + 1, q - 1, success);
    }

    int op = find_main_op(p, q);
    if (op == -1) {
        *success = false;
        return 0;
    }

    if (tokens[op].type == TK_NEG || tokens[op].type == TK_DEREF) {
        // 单目运算符
        uint32_t val = eval(op + 1, q, success);
        if (!*success) return 0;
        if (tokens[op].type == TK_NEG) {
            return -val;
        } else { // TK_DEREF
            return npc_pmem_read(val);
        }
    }

    // 双目运算符
    uint32_t val1 = eval(p, op - 1, success);
    if (!*success) return 0;
    uint32_t val2 = eval(op + 1, q, success);
    if (!*success) return 0;

    switch (tokens[op].type) {
        case TK_PLUS:  return val1 + val2;
        case TK_MINUS: return val1 - val2;
        case TK_MUL:   return val1 * val2;
        case TK_DIV:   return val1 / val2;
        case TK_EQ:    return val1 == val2;
        case TK_NEQ:   return val1 != val2;
        case TK_AND:   return val1 && val2;
        default:       *success = false; return 0;
    }
}

// 表达式求值入口
uint32_t expr_eval(const char* expr, bool* success) {
    *success = tokenize(expr);
    if (!*success) return 0;
    return eval(0, tokens.size() - 1, success);
}