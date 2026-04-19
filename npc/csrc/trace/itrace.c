#include "trace/itrace.h"
#include <stdio.h>

#if NPC_ENABLE_ITRACE

#include <llvm-c/Disassembler.h>
#include <llvm-c/Target.h>
#include <llvm-c/TargetMachine.h>

#define IRINGBUF_SIZE 16 //环形缓冲区容量
#define ITRACE_DISASM_LEN 128

typedef struct {
  uint32_t pc;   //程序计数器地址
  uint32_t inst; //原始指令机器码
  char disasm[ITRACE_DISASM_LEN]; //反汇编结果字符串
} ItraceRecord;

static ItraceRecord iringbuf[IRINGBUF_SIZE]; //指令环形缓冲区
static int iring_head = 0; //当前写入位置（下一个空槽）
static bool iring_full = false;

static LLVMDisasmContextRef disasm = NULL; //LLVM反汇编器上下文

//判断itrace是否处于可工作状态
static inline bool itrace_ready(void) {
  return disasm != NULL;
}

static inline int iring_count(void) {
  return iring_full ? IRINGBUF_SIZE : iring_head;
}

static inline int iring_last_idx(void) {
  return (iring_head + IRINGBUF_SIZE - 1) % IRINGBUF_SIZE;
}

static void iring_push(uint32_t pc, uint32_t inst, const char *disasm_text) {
  //记录指令到环形缓冲区
  ItraceRecord *slot = &iringbuf[iring_head];
  slot->pc = pc;
  slot->inst = inst;
  snprintf(slot->disasm, sizeof(slot->disasm), "%s", disasm_text);

  iring_head = (iring_head + 1) % IRINGBUF_SIZE;
  if (iring_head == 0) {
    iring_full = true;
  }
}


void itrace_init(NPCContext* ctx) {
  (void)ctx;

  //注册目标信息/MC层/反汇编器工厂
  LLVMInitializeAllTargetInfos();
  LLVMInitializeAllTargetMCs();
  LLVMInitializeAllDisassemblers();

  //创建riscv32目标的反汇编器上下文
  disasm = LLVMCreateDisasmCPU("riscv32", "", NULL, 0, NULL, NULL);
  if (!disasm) {
    printf("itrace: Failed to create LLVM C disassembler\n");
    return;
  }

  iring_head = 0;
  iring_full = false;

  printf("Instruction tracing initialized with LLVM(C API)\n");
}


static bool disassemble_to_string(uint64_t pc, uint32_t inst, char *out, size_t out_size) {
  if (!itrace_ready()) {
    return false;
  }

  //对RV32固定长度指令，成功时通常返回4，失败时返回0
  size_t n = LLVMDisasmInstruction(disasm, (uint8_t *)&inst, 4, pc, out, out_size);
  return n != 0;
}


void itrace_disassemble(uint64_t pc, uint32_t inst) {
  char asm_str[128] = {0};
  if (!disassemble_to_string(pc, inst, asm_str, sizeof(asm_str))) {
    return;
  }
  printf("[itrace] 0x%08lx: %s\n", (unsigned long)pc, asm_str);
}


void itrace_step(NPCContext* ctx) {
  if (!ctx->debug.itrace_enabled) {
    return;
  }
  if (!itrace_ready()) {
    return;
  }

  uint32_t pc = ctx->pc;
  uint32_t inst = ctx->inst;

  //过滤占位码
  if (inst == 0xFFFFFFFFu) {
    return;
  }

  char asm_str[128] = {0};
  if (!disassemble_to_string(pc, inst, asm_str, sizeof(asm_str))) {
    return;
  }

  //记录最近指令,用于失败后回看上下文
  iring_push(pc, inst, asm_str);

  printf("[itrace] 0x%08x: %s\n", pc, asm_str);
}


void itrace_dump_iringbuf(void) {
  //显示指令环形缓冲区内容
  int count = iring_count();
  if (count == 0) {
    printf("(iringbuf) no instructions recorded.\n");
    return;
  }

  int start = iring_full ? iring_head : 0;
  int last_idx = iring_last_idx();

  for (int i = 0; i < count; i++) {
    int idx = (start + i) % IRINGBUF_SIZE;
    ItraceRecord *rec = &iringbuf[idx];
    const char *mark = (idx == last_idx) ? "-->" : "   ";

    printf("%s 0x%08x: %-32s  %02x %02x %02x %02x\n",mark,rec->pc,rec->disasm,
           (rec->inst >> 24) & 0xff,(rec->inst >> 16) & 0xff,(rec->inst >>  8) & 0xff,(rec->inst >>  0) & 0xff);
  }
}

#else

//关闭itrace时提供空实现,避免上层条件编译分散
void itrace_init(NPCContext* ctx)                  { (void)ctx; }
void itrace_disassemble(uint64_t pc, uint32_t inst){ (void)pc; (void)inst; }
void itrace_step(NPCContext* ctx)                  { (void)ctx; }
void itrace_dump_iringbuf(void)                    {}

#endif
