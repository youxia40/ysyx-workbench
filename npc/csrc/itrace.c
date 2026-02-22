#include "itrace.h"
#include <stdio.h>



#if NPC_ENABLE_ITRACE

#include <llvm-c/Disassembler.h>
#include <llvm-c/Target.h>
#include <llvm-c/TargetMachine.h>

#define IRINGBUF_SIZE 16
#define ITRACE_DISASM_LEN 128

typedef struct {//指令追踪记录结构体,保存PC/指令/反汇编字符串
  uint32_t pc;
  uint32_t inst;
  char disasm[ITRACE_DISASM_LEN];//反汇编结果字符串
} ItraceRecord;

static ItraceRecord iringbuf[IRINGBUF_SIZE];//最近指令环形缓冲
static int iring_head = 0;//下一个写入位置
static bool iring_full = false;//是否已经写满一圈


static LLVMDisasmContextRef disasm = NULL;//LLVM反汇编器上下文,全局

//判断itrace是否处于可工作状态
static inline bool itrace_ready(void) {
  return disasm != NULL;
}

static inline int iring_count(void) {//当前有效记录数
  return iring_full ? IRINGBUF_SIZE : iring_head;
}

static inline int iring_last_idx(void) {//最近一条记录的下标
  return (iring_head + IRINGBUF_SIZE - 1) % IRINGBUF_SIZE;
}

//向环形缓冲写入一条记录
static void iring_push(uint32_t pc, uint32_t inst, const char *disasm_text) {
  ItraceRecord *slot = &iringbuf[iring_head];//写入当前记录到head位置
  slot->pc = pc;
  slot->inst = inst;
  snprintf(slot->disasm, sizeof(slot->disasm), "%s", disasm_text);//安全复制反汇编字符串

  iring_head = (iring_head + 1) % IRINGBUF_SIZE;//更新head指向下一个写入位置
  if (iring_head == 0) {
    iring_full = true;
  }
}


//初始化LLVM目标信息并创建RISC-V32反汇编器
void itrace_init(NPCContext* ctx) {
  (void)ctx;//统一接口保留ctx参数，当前实现不使用，但可以在这里根据ctx信息定制反汇编器配置

  //注册目标信息/MC层/反汇编器工厂
  LLVMInitializeAllTargetInfos();
  LLVMInitializeAllTargetMCs();
  LLVMInitializeAllDisassemblers();
  //LLVM C反汇编API的标准初始化序列



  //创建riscv32目标的反汇编器上下文
  //CPU字符串为空表示使用默认子架构配置
  disasm = LLVMCreateDisasmCPU("riscv32", "", NULL, 0, NULL, NULL);
  //参数:目标三元组、CPU特征、外部符号回调及其上下文、日志回调及其上下文
  if (!disasm) {
    printf("itrace: Failed to create LLVM C disassembler\n");
    return;
  }

  iring_head = 0;
  iring_full = false;

  printf("Instruction tracing initialized with LLVM(C API)\n");
}


//把一条32位机器码反汇编为字符串
//返回true表示成功解码并写入out缓冲区
static bool disassemble_to_string(uint64_t pc, uint32_t inst, char *out, size_t out_size) {
  if (!itrace_ready()) {
    return false;
  }

  //LLVMDisasmInstruction返回“解码消耗的字节数”
  //对RV32固定长度指令来说,成功时通常为4,失败时为0
  size_t n = LLVMDisasmInstruction(disasm, (uint8_t *)&inst, 4, pc, out, out_size);
  return n != 0;//成功解码返回true,失败返回false
}


//反汇编并直接打印一条指定pc/inst
void itrace_disassemble(uint64_t pc, uint32_t inst) {
  char asm_str[128] = {0};
  if (!disassemble_to_string(pc, inst, asm_str, sizeof(asm_str))) {
    return;
  }
  printf("[itrace] 0x%08lx: %s\n", (unsigned long)pc, asm_str);
}


//按当前NPC上下文记录并打印一条指令，先由DPI-C回调先写入ctx->pc/ctx->inst,再调用本函数
void itrace_step(NPCContext* ctx) {
  if (!ctx->debug.itrace_enabled) {
    return;
  }
  if (!itrace_ready()) {//反汇编器未准备好则直接返回
    return;
  }

  uint32_t pc = ctx->pc;
  uint32_t inst = ctx->inst;

  //过滤占位码
  if (inst == 0xFFFFFFFFu) {
    return;
  }

  char asm_str[128] = {0};//反汇编结果缓冲区
  if (!disassemble_to_string(pc, inst, asm_str, sizeof(asm_str))) {
    return;
  }

  //记录最近指令,用于失败后回看上下文
  iring_push(pc, inst, asm_str);

  printf("[itrace] 0x%08x: %s\n", pc, asm_str);
}


void itrace_dump_iringbuf(void) {//打印最近记录的指令环形缓冲区,便于失败诊断
  int count = iring_count();//当前有效记录数
  if (count == 0) {
    printf("(iringbuf) no instructions recorded.\n");
    return;
  }

  int start = iring_full ? iring_head : 0;//按时间顺序从最旧记录开始
  int last_idx = iring_last_idx();//最近一条记录

  for (int i = 0; i < count; i++) {
    int idx = (start + i) % IRINGBUF_SIZE;//当前记录下标
    ItraceRecord *rec = &iringbuf[idx];//当前记录
    const char *mark = (idx == last_idx) ? "-->" : "   ";

    printf("%s 0x%08x: %-32s  %02x %02x %02x %02x\n",mark,rec->pc,rec->disasm,
           (rec->inst >> 24) & 0xff,(rec->inst >> 16) & 0xff,(rec->inst >>  8) & 0xff,(rec->inst >>  0) & 0xff);
           //标记PC地址反汇编字符串机器码
  }
}

#else

//关闭itrace时提供空实现,避免上层条件编译分散
void itrace_init(NPCContext* ctx)                  { (void)ctx; }
void itrace_disassemble(uint64_t pc, uint32_t inst){ (void)pc; (void)inst; }
void itrace_step(NPCContext* ctx)                  { (void)ctx; }
void itrace_dump_iringbuf(void)                    {}

#endif
