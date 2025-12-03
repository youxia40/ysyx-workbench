#include "difftest.h"
#include "common.h"
#include "dpi.h"
#include "mem.h"

#if NPC_ENABLE_DIFFTEST

#include <dlfcn.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <unistd.h>

// Must match NEMU's difftest-def.h
enum { DIFFTEST_TO_DUT = 0, DIFFTEST_TO_REF = 1 };

// NEMU difftest API types
typedef void (*ref_init_t)(int port);
typedef void (*ref_memcpy_t)(uint32_t addr, void *buf, size_t n, bool direction);
typedef void (*ref_regcpy_t)(void *dut, bool direction);
typedef void (*ref_exec_t)(uint64_t n);
typedef void (*ref_raise_intr_t)(uint64_t NO);

// Shared library handle and function pointers
static void *ref_handle          = nullptr;
static ref_init_t       ref_init = nullptr;
static ref_memcpy_t     ref_memcpy = nullptr;
static ref_regcpy_t     ref_regcpy = nullptr;
static ref_exec_t       ref_exec = nullptr;
static ref_raise_intr_t ref_raise_intr = nullptr;

// A CPU state compatible with NEMU's riscv32_CPU_state:
// 32 GPRs + PC
typedef struct {
    uint32_t gpr[32];
    uint32_t pc;
} DifftestCPUState;

// Collect DUT(NPC) state into DifftestCPUState
static void npc_collect_cpu_state(DifftestCPUState *st, NPCContext *ctx) {
    uint32_t gpr16[16] = {0};
    npc_get_regs(gpr16);  // get 16 RV32E registers from NPC

    for (int i = 0; i < 16; i++) {
        st->gpr[i] = gpr16[i];
    }
    // RV32E only has 16 GPRs, the rest are always 0
    for (int i = 16; i < 32; i++) {
        st->gpr[i] = 0;
    }
    st->pc = ctx->pc;  // NPC's PC after the current instruction
}

// Try multiple paths to open NEMU so
static void* try_open_so(const char **used_path) {
    void *handle = nullptr;
    const char *env_path = getenv("NEMU_SHARED_LIB");

    if (env_path && env_path[0] != '\0') {
        handle = dlopen(env_path, RTLD_LAZY);
        if (handle) {
            *used_path = env_path;
            return handle;
        } else {
            printf("DiffTest: dlopen(%s) failed: %s\n", env_path, dlerror());
        }
    }

    static const char* candidates[] = {
        "nemu/build/riscv32-nemu-interpreter-so",
        "../nemu/build/riscv32-nemu-interpreter-so",
        "../../nemu/build/riscv32-nemu-interpreter-so",
        "../../../nemu/build/riscv32-nemu-interpreter-so",
        nullptr
    };

    for (int i = 0; candidates[i]; i++) {
        const char* p = candidates[i];
        handle = dlopen(p, RTLD_LAZY);
        if (handle) {
            *used_path = p;
            return handle;
        }
    }

    *used_path = nullptr;
    return nullptr;
}

// Initialize difftest: load NEMU so, sync memory and registers
void difftest_init(NPCContext* ctx) {
    if (!ctx->debug.difftest_enabled) {
        return;
    }

    const char *used_path = nullptr;
    ref_handle = try_open_so(&used_path);

    if (!ref_handle) {
        char cwd_buf[512];
        const char *cwd = getcwd(cwd_buf, sizeof(cwd_buf));
        fprintf(stderr,
                "DiffTest: failed to load NEMU shared object.\n"
                "Current working directory: %s\n"
                "Tried paths:\n"
                "  $NEMU_SHARED_LIB (if set)\n"
                "  nemu/build/riscv32-nemu-interpreter-so\n"
                "  ../nemu/build/riscv32-nemu-interpreter-so\n"
                "  ../../nemu/build/riscv32-nemu-interpreter-so\n"
                "  ../../../nemu/build/riscv32-nemu-interpreter-so\n",
                cwd ? cwd : "(unknown)");
        // Do not continue with a half-enabled difftest
        exit(1);
    }

    ref_init       = (ref_init_t)      dlsym(ref_handle, "difftest_init");
    ref_memcpy     = (ref_memcpy_t)    dlsym(ref_handle, "difftest_memcpy");
    ref_regcpy     = (ref_regcpy_t)    dlsym(ref_handle, "difftest_regcpy");
    ref_exec       = (ref_exec_t)      dlsym(ref_handle, "difftest_exec");
    ref_raise_intr = (ref_raise_intr_t)dlsym(ref_handle, "difftest_raise_intr");

    if (!ref_init || !ref_memcpy || !ref_regcpy || !ref_exec) {
        printf("DiffTest: missing symbol(s) in NEMU shared object\n");
        dlclose(ref_handle);
        ref_handle = nullptr;
        exit(1);
    }

    printf("DiffTest: using NEMU so: %s\n", used_path ? used_path : "(env)");

    // Initialize REF (NEMU)
    ref_init(0);

    // Sync initial memory:
    // For simplicity, copy first 2MB from MEM_BASE to NEMU.
    const uint32_t img_sync_size = 2 * 1024 * 1024; // 2MB
    for (uint32_t off = 0; off < img_sync_size; off += 4) {
        // npc_pmem_read uses physical offset (virt - MEM_BASE)
        uint32_t word = npc_pmem_read((int)off);
        ref_memcpy(MEM_BASE + off, &word, 4, DIFFTEST_TO_REF);
    }

    // Sync initial registers + PC to REF.
    // NOTE: At this moment ctx->pc is still 0, so we must use ctx->entry.
    DifftestCPUState cpu_init{};
    for (int i = 0; i < 32; i++) {
        cpu_init.gpr[i] = 0;      // all GPRs start from 0
    }
    cpu_init.pc = ctx->entry;     // initial PC = program entry (e.g. 0x80000000)

    ref_regcpy(&cpu_init, DIFFTEST_TO_REF);

    printf("DiffTest initialized successfully\n");
}

// Single-step difftest
void difftest_step(NPCContext* ctx) {
    if (!ctx->debug.difftest_enabled) return;
    if (!ref_handle) return;

    // REF state BEFORE executing this step:
    // This PC is the instruction address we are going to execute now.
    DifftestCPUState ref_before{};
    ref_regcpy(&ref_before, DIFFTEST_TO_DUT);

    // Let REF execute one instruction to catch up with DUT
    ref_exec(1);

    // REF state AFTER this instruction
    DifftestCPUState ref_after{};
    ref_regcpy(&ref_after, DIFFTEST_TO_DUT);

    // DUT state AFTER this instruction
    DifftestCPUState dut_after{};
    npc_collect_cpu_state(&dut_after, ctx);

    // The instruction that likely caused a mismatch has this PC
    uint32_t instr_pc = ref_before.pc;

    // First compare PC after executing the instruction
    if (ref_after.pc != dut_after.pc) {
        printf("[DiffTest] PC mismatch at instruction PC = 0x%08x\n", instr_pc);
        printf("           Next PC after this instruction: DUT=0x%08x, REF=0x%08x\n",
               dut_after.pc, ref_after.pc);
        printf("Hint: This usually indicates a bug in control-flow logic, e.g.\n");
        printf("      - wrong jump/branch target calculation\n");
        printf("      - incorrect pc+4 update\n");
        printf("      - different behavior on ebreak/exception/return\n");
        ctx->stop = true;
        ctx->stop_reason = (char*)"DiffTest PC mismatch";
        return;
    }

    // Then compare 16 RV32E registers x0..x15
    for (int i = 0; i < 16; i++) {
        if (ref_after.gpr[i] != dut_after.gpr[i]) {
            printf("[DiffTest] Register mismatch at instruction PC = 0x%08x\n",
                   instr_pc);
            printf("           x%-2d: DUT=0x%08x, REF=0x%08x\n",
                   i, dut_after.gpr[i], ref_after.gpr[i]);
            printf("Hint: The bug is very likely in the implementation of\n");
            printf("      the instruction at 0x%08x, or in the write-back\n", instr_pc);
            printf("      path for register x%d (ALU result, load data, etc.).\n", i);
            ctx->stop = true;
            ctx->stop_reason = (char*)"DiffTest reg mismatch";
            return;
        }
    }
}

#else  // NPC_ENABLE_DIFFTEST == 0

void difftest_init(NPCContext* ctx) { (void)ctx; }
void difftest_step(NPCContext* ctx) { (void)ctx; }

#endif