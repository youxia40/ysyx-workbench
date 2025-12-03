#include "sdb.h"
#include "common.h"
#include "expr.h"
#include "watchpoint.h"
#include "dpi.h"
#include <readline/readline.h>
#include <readline/history.h>
#include <cstring>
#include <cstdlib>
#include <cctype>
#include <vector>
#include <map>
#include <cstdio>

// 打印帮助信息
void sdb_help() {
    printf("Available commands:\n");
    printf("  help - Display this help message\n");
    printf("  c - Continue execution until next breakpoint\n");
    printf("  q - Quit NPC simulator\n");
    printf("  si [N] - Execute N instructions (default 1)\n");
    printf("  info r - Show register values\n");
    printf("  info w - Show watchpoints\n");
    printf("  x N EXPR - Examine memory at EXPR for N words\n");
    printf("  p EXPR - Evaluate expression EXPR\n");
    printf("  w EXPR - Set watchpoint for expression EXPR\n");
    printf("  d N - Delete watchpoint N\n");
    printf("  pt - Run expression test\n");
}

// 打印寄存器
void sdb_print_registers() {
    uint32_t regs[REG_NUM];
    npc_get_regs(regs);
    
    printf("Registers:\n");
    for (int i = 0; i < REG_NUM; i++) {
        printf("%-4s (x%2d): 0x%08x", reg_names[i], i, regs[i]);
        if (i % 4 == 3) printf("\n");
    }
}

// 扫描内存
void sdb_scan_memory(int count, uint32_t addr) {
    printf("Memory at 0x%08X:\n", addr);
    for (int i = 0; i < count; i++) {
        uint32_t data = npc_pmem_read(addr + i * 4);
        printf("0x%08X: 0x%08X\n", addr + i * 4, data);
    }
}

// 表达式求值
void sdb_eval_expr(const char* expr) {
    bool success = false;
    uint32_t result = expr_eval(expr, &success);
    if (success) {
        printf("%s = %d (0x%08X)\n", expr, result, result);
    } else {
        printf("Failed to evaluate expression: %s\n", expr);
    }
}

// 表达式测试
void sdb_run_expr_test() {
    const char* input_file = "input";
    FILE* fp = fopen(input_file, "r");
    if (!fp) {
        printf("Error! File %s not found!\n", input_file);
        return;
    }

    char line[256];
    unsigned total = 0, passed = 0;
    char error_log[4096] = {0};
    size_t error_offset = 0;

    while (fgets(line, sizeof(line), fp)) {
        total++;
        line[strcspn(line, "\r\n")] = '\0'; // 移除换行符

        uint32_t expected, actual;
        char expr[256];
        if (sscanf(line, "%u %[^\n]", &expected, expr) != 2) {
            error_offset += snprintf(error_log + error_offset, sizeof(error_log) - error_offset,
                "[Line %u] Bad format: %s\n", total, line);
            continue;
        }

        bool success = false;
        actual = expr_eval(expr, &success);
        if (!success) {
            error_offset += snprintf(error_log + error_offset, sizeof(error_log) - error_offset,
                "[Line %u] Invalid expression: %s\n", total, line);
            continue;
        }

        if (expected != actual) {
            error_offset += snprintf(error_log + error_offset, sizeof(error_log) - error_offset,
                "[Line %u] Mismatch: %s\n"
                "  Expected: %u (0x%08X)\n"
                "  Actual  : %u (0x%08X)\n",
                total, line, expected, expected, actual, actual);
            continue;
        }

        passed++;
    }

    fclose(fp);

    if (error_offset > 0) {
        printf("\nError Report:\n%s", error_log);
    }
    printf("\nTest finished: %u/%u passed\n", passed, total);
}

// SDB主循环
void sdb_step(NPCContext* ctx) {
    char* line = readline("(sdb) ");
    if (!line) return;
    
    add_history(line);
    
    char* cmd = strtok(line, " ");
    if (!cmd) {
        free(line);
        return;
    }
    
    if (strcmp(cmd, "help") == 0) {
        sdb_help();
    } else if (strcmp(cmd, "c") == 0) {
        ctx->debug.sdb_enabled = false; // 修复：使用debug.sdb_enabled
        printf("Continuing execution...\n");
    } else if (strcmp(cmd, "q") == 0) {
        ctx->stop = true;
        ctx->stop_reason = "User quit";
        printf("Exiting NPC simulator\n");
    } else if (strcmp(cmd, "si") == 0) {
        char* n_str = strtok(NULL, " ");
        int n = n_str ? atoi(n_str) : 1;
        if (n > 0) {
            ctx->debug.sdb_step_count = n; // 修复：使用debug.sdb_step_count
            printf("Stepping %d instruction(s)\n", n);
        } else {
            printf("Invalid step count: %s\n", n_str);
        }
    } else if (strcmp(cmd, "info") == 0) {
        char* subcmd = strtok(NULL, " ");
        if (subcmd) {
            if (strcmp(subcmd, "r") == 0) {
                sdb_print_registers();
            } else if (strcmp(subcmd, "w") == 0) {
                list_watchpoints(); // 修复：使用正确的函数名
            } else {
                printf("Unknown info subcommand: %s\n", subcmd);
            }
        } else {
            printf("Missing info subcommand\n");
        }
    } else if (strcmp(cmd, "x") == 0) {
        char* n_str = strtok(NULL, " ");
        char* expr = strtok(NULL, "");
        if (n_str && expr) {
            int n = atoi(n_str);
            if (n > 0) {
                bool success = false;
                uint32_t addr = expr_eval(expr, &success);
                if (success) {
                    sdb_scan_memory(n, addr);
                } else {
                    printf("Invalid expression: %s\n", expr);
                }
            } else {
                printf("Invalid count: %s\n", n_str);
            }
        } else {
            printf("Missing arguments for x command\n");
        }
    } else if (strcmp(cmd, "p") == 0) {
        char* expr = strtok(NULL, "");
        if (expr) {
            sdb_eval_expr(expr);
        } else {
            printf("Missing expression for p command\n");
        }
    } else if (strcmp(cmd, "w") == 0) {
        char* expr = strtok(NULL, "");
        if (expr) {
            Watchpoint* wp = new_watchpoint(); // 修复：使用new_watchpoint
            if (wp) {
                // 复制表达式
                strncpy(wp->expr, expr, sizeof(wp->expr)-1);
                wp->expr[sizeof(wp->expr)-1] = '\0';
                // 计算初始值
                wp->value = expr_eval(expr, NULL);
                printf("Watchpoint %d added: %s\n", wp->id, wp->expr);
            } else {
                printf("Failed to add watchpoint\n");
            }
        } else {
            printf("Missing expression for w command\n");
        }
    } else if (strcmp(cmd, "d") == 0) {
        char* id_str = strtok(NULL, " ");
        if (id_str) {
            int id = atoi(id_str);
            free_watchpoint(id); // 修复：使用free_watchpoint
            printf("Watchpoint %d deleted\n", id);
        } else {
            printf("Missing watchpoint ID for d command\n");
        }
    } else if (strcmp(cmd, "pt") == 0) {
        sdb_run_expr_test();
    } else {
        printf("Unknown command: %s\n", cmd);
    }
    
    free(line);
}

// 初始化SDB
void sdb_init(NPCContext* ctx) {
    init_watchpoints();
    printf("SDB initialized. Type 'help' for commands.\n");
}

// 检查监视点
void sdb_check_watchpoints(NPCContext* ctx) {
    if (check_watchpoints()) {
        ctx->debug.sdb_enabled = true; // 修复：使用debug.sdb_enabled
        printf("Watchpoint triggered. Entering debug mode.\n");
    }
}