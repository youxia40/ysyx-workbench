#include "csrcn/sim_core.h"
#include "csrcn/sdb.h"
#include "csrcn/trace.h"
#include "csrcn/difftest.h"
#include <cstdio>
#include <cstdlib>
#include <dlfcn.h>

// 关闭 DiffTest 的辅助函数
void close_diff_test(SimContext* sim) {
    if (sim->diff_test_enabled && sim->nemu_handle) {
        auto diff_test_fini = (void(*)())dlsym(sim->nemu_handle, "difftest_fini");
        if (diff_test_fini) diff_test_fini();
        
        dlclose(sim->nemu_handle);
        sim->nemu_handle = nullptr;
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <program.bin> [max_cycles]\n", argv[0]);
        return 1;
    }

    const char* program_path = argv[1];
    int max_cycles = 100000;
    if (argc > 2) {
        max_cycles = atoi(argv[2]);
        if (max_cycles <= 0) {
            fprintf(stderr, "Invalid max_cycles: %s\n", argv[2]);
            return 1;
        }
    }

    // 初始化仿真环境
    SimContext* sim = sim_init();
    if (!sim) {
        fprintf(stderr, "Failed to initialize simulation context\n");
        return 1;
    }

    // 加载程序
    if (sim_load_program(sim, program_path) != 0) {
        fprintf(stderr, "Failed to load program: %s\n", program_path);
        sim_free(sim);
        return 1;
    }

    // =================================================
    // 默认开启 DiffTest（强制开启）
    // =================================================
    sim->diff_test_enabled = true;  // 强制开启标志
    
    // 尝试初始化，失败时继续运行但禁用功能
    if (!init_diff_test(sim)) {
        fprintf(stderr, "[WARN] DiffTest initialization failed, continuing without it\n");
        sim->diff_test_enabled = false;  // 禁用功能
    } else {
        printf("[INFO] DiffTest initialized successfully\n");
    }

    // =================================================
    // 默认开启 Trace（强制开启）
    // =================================================
    // 初始化 Trace 系统（不需要检查返回值）
    init_trace(sim);
    
    // 初始化 SDB（重要修复：传递上下文指针）
    init_sdb(sim); // 传递全局仿真上下文

    // 输出配置信息（显示 Trace 和 DiffTest 状态）
    printf("\nNPC Simulation Started. Program: %s\n", program_path);
    printf("Configuration:\n");
    printf("  - Trace:    ON (default)\n");  // 强制显示为开启状态
    printf("  - DiffTest: %s\n", sim->diff_test_enabled ? "ON" : "OFF (initialization failed)");
    printf("Executing up to %d cycles...\n\n", max_cycles);
    
    // 启动主循环（使用传递的上下文）
    sdb_mainloop(sim, max_cycles);

    // 统一在主函数中释放资源
    if (sim) {
        // 关闭DiffTest
        if (sim->diff_test_enabled && sim->nemu_handle) {
            auto diff_test_fini = (void(*)())dlsym(sim->nemu_handle, "difftest_fini");
            if (diff_test_fini) diff_test_fini();
            dlclose(sim->nemu_handle);
            sim->nemu_handle = nullptr;
        }
        
        // 关闭跟踪系统
        close_trace();
        
        // 释放内存
        if (sim->memory) {
            delete[] sim->memory;
            sim->memory = nullptr;
        }
        
        // 保存结果用于输出
        const uint32_t final_pc = sim->pc;
        const uint64_t total_insts = sim->total_instructions;
        const bool good_trap = sim->hit_good_trap;
        const bool bad_trap = sim->hit_bad_trap;
        
        // 最后删除模拟器上下文
        delete sim;
        
        // 输出结果
        printf("\n=== Simulation Result ===\n");
        if (good_trap) {
            printf("GOOD TRAP at pc=0x%08x\n", final_pc);
        } else if (bad_trap) {
            printf("BAD TRAP at pc=0x%08x\n", final_pc);
        } else if (total_insts >= max_cycles) {
            printf("Reached max cycles (%d)\n", max_cycles);
        } else {
            printf("Stopped by user\n");
        }
        printf("Instructions executed: %lu\n", total_insts);
    }
    
    return 0;
}