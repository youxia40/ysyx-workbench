#include "difftest.h"
#include "sim_core.h"
#include <dlfcn.h>
#include <cstdio>
#include <cstring>

typedef void (*regcpy_t)(void*, bool);  // NEMU寄存器复制函数（void* ctx, bool is_ref）
typedef void (*exec_t)(uint64_t);       // NEMU执行函数（uint64_t cycles）

static regcpy_t nemu_regcpy = nullptr;  // NEMU寄存器复制函数指针
static exec_t nemu_exec = nullptr;      // NEMU执行函数指针
static void* nemu_handle = nullptr;     // NEMU共享库句柄

bool init_diff_test(SimContext* sim) {
    if (sim->nemu_handle) {
        return true; // 已初始化
    }
    
    // 加载NEMU共享库
    const char* nemu_lib_path = getenv("NEMU_SHARED_LIB");
    if (!nemu_lib_path) {
        nemu_lib_path = "/home/pz40/ysyx-workbench/nemu/build/riscv32-nemu-interpreter-so";
    }
    
    sim->nemu_handle = dlopen(nemu_lib_path, RTLD_LAZY);
    if (!sim->nemu_handle) {
        fprintf(stderr, "Failed to load NEMU: %s\n", dlerror());
        return false;
    }
    
    // 获取函数指针
    nemu_regcpy = (regcpy_t)dlsym(sim->nemu_handle, "difftest_regcpy");
    nemu_exec = (exec_t)dlsym(sim->nemu_handle, "difftest_exec");
    
    if (!nemu_regcpy || !nemu_exec) {
        fprintf(stderr, "Failed to find difftest functions: %s\n", dlerror());
        dlclose(sim->nemu_handle);
        sim->nemu_handle = nullptr;
        return false;
    }
    
    // 同步初始状态（从NPC复制寄存器到NEMU）
    uint32_t npc_regs[32];
    get_regs(npc_regs); // 获取NPC寄存器值
    nemu_regcpy(npc_regs, false); // 传递给NEMU（非参考模式）
    
    return true;
}

bool do_diff_test(SimContext* sim) {
    if (!sim || !sim->diff_test_enabled) return true;
    
    // 获取 NEMU 状态
    auto diff_test_exec = (void(*)(uint64_t))dlsym(sim->nemu_handle, "difftest_exec");
    if (!diff_test_exec) return false;
    
    // 执行 DiffTest
    diff_test_exec(1); // 执行一条指令
    
    // 获取 NEMU 的 PC
    auto diff_test_regcpy = (void(*)(void*, bool))dlsym(sim->nemu_handle, "difftest_regcpy");
    if (!diff_test_regcpy) return false;
    
    uint32_t nemu_pc;
    diff_test_regcpy(&nemu_pc, false); // false 表示从 NEMU 复制到本地
    
    // 检查 PC 一致性
    if (nemu_pc != sim->pc) {
        fprintf(stderr, 
            "[DIFFTEST] PC mismatch: NPC=0x%08x, NEMU=0x%08x\n",
            sim->pc, nemu_pc
        );
        
        // 标记差异来源
        sim->pc_modifier = 1; // 1 表示 DiffTest 修改
    }
    
    return true;
}