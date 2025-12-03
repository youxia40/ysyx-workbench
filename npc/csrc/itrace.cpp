#include "itrace.h"
#include "common.h"

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <string>

#if NPC_ENABLE_ITRACE

#include <llvm/MC/MCDisassembler/MCDisassembler.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/MC/MCInstPrinter.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/MC/MCInst.h>
#include <llvm/ADT/Triple.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/MC/MCSubtargetInfo.h>
#include <llvm/MC/MCContext.h>
#include <llvm/MC/MCAsmInfo.h>
#include <llvm/MC/MCInstrInfo.h>
#include <llvm/MC/MCRegisterInfo.h>
#include <llvm/MC/MCTargetOptions.h>

// 使用 llvm_ctx 避免与 NPCContext 冲突
static llvm::MCDisassembler*  disas      = nullptr;
static llvm::MCInstPrinter*   printer    = nullptr;
static llvm::MCContext*       llvm_ctx   = nullptr;
static llvm::MCSubtargetInfo* subtarget  = nullptr;
static llvm::MCAsmInfo*       asm_info   = nullptr;
static llvm::MCInstrInfo*     instr_info = nullptr;
static llvm::MCRegisterInfo*  reg_info   = nullptr;

//环形缓冲区长度
static const int IRINGBUF_SIZE = 16;

struct ItraceRecord {
  uint32_t    pc;
  uint32_t    inst;
  std::string disasm;
};

static ItraceRecord iringbuf[IRINGBUF_SIZE];
static int  iring_head = 0;   // 下一个写入的位置
static bool iring_full = false;

// 初始化指令追踪（LLVM 反汇编）
void itrace_init(NPCContext* ctx) {
  (void)ctx; // 目前没用到 ctx

  llvm::InitializeAllTargetInfos();
  llvm::InitializeAllTargetMCs();
  llvm::InitializeAllDisassemblers();

  std::string error;
  const llvm::Target* target =
      llvm::TargetRegistry::lookupTarget("riscv32", error);
  if (!target) {
    printf("itrace: Failed to init disassembler: %s\n", error.c_str());
    return;
  }

  llvm::Triple triple("riscv32");
  llvm::MCTargetOptions options;

  reg_info   = target->createMCRegInfo(triple.str());
  asm_info   = target->createMCAsmInfo(*reg_info, triple.str(), options);
  instr_info = target->createMCInstrInfo();
  subtarget  = target->createMCSubtargetInfo(triple.str(), "", "");
  llvm_ctx   = new llvm::MCContext(triple, asm_info, reg_info, subtarget);

  disas   = target->createMCDisassembler(*subtarget, *llvm_ctx);
  printer = target->createMCInstPrinter(triple, 0,
                                        *asm_info, *instr_info, *reg_info);

  if (!disas || !printer) {
    printf("itrace: Failed to create disassembler/printer\n");
    return;
  }

  iring_head = 0;
  iring_full = false;

  printf("Instruction tracing initialized with LLVM (iringbuf size = %d)\n",
         IRINGBUF_SIZE);
}

// 反汇编一条指令，得到字符串（不打印）
static bool disassemble_to_string(uint64_t pc, uint32_t inst, std::string &out) {
  if (!disas || !printer) return false;

  llvm::MCInst mcinst;
  uint64_t size = 0;
  llvm::ArrayRef<uint8_t> bytes(reinterpret_cast<uint8_t*>(&inst), 4);

  bool ok = disas->getInstruction(mcinst, size, bytes, pc, llvm::nulls());
  if (!ok) return false;

  out.clear();
  llvm::raw_string_ostream os(out);
  printer->printInst(&mcinst, pc, "", *subtarget, os);
  os.flush();
  return true;
}

// 保留这个接口（如果以后需要实时打印单条可以用）
void itrace_disassemble(uint64_t pc, uint32_t inst) {
  std::string asm_str;
  if (!disassemble_to_string(pc, inst, asm_str)) return;
  printf("[itrace] 0x%08lx: %s\n", (unsigned long)pc, asm_str.c_str());
}

// 每条指令后调用：只负责往 iringbuf 里塞记录
void itrace_step(NPCContext* ctx) {
  if (!ctx->debug.itrace_enabled) return;
  if (!disas || !printer) return;

  uint32_t pc   = ctx->pc;
  uint32_t inst = ctx->inst;

  // 简单过滤非法指令占位
  if (inst == 0xFFFFFFFFu) {
    return;
  }

  std::string asm_str;
  if (!disassemble_to_string(pc, inst, asm_str)) {
    return;
  }

  ItraceRecord &slot = iringbuf[iring_head];
  slot.pc     = pc;
  slot.inst   = inst;
  slot.disasm = asm_str;

  iring_head = (iring_head + 1) % IRINGBUF_SIZE;
  if (iring_head == 0) {
    iring_full = true;
  }
}

// 在测试失败等情况下调用：按讲义风格打印 iringbuf
void itrace_dump_iringbuf(void) {
  if (!disas || !printer) return;

  int count = iring_full ? IRINGBUF_SIZE : iring_head;
  if (count == 0) {
    printf("(iringbuf) no instructions recorded.\n");
    return;
  }

  // 最后一条是“最近执行”的指令，我们假定它是出错指令，用 --> 标记
  int last_idx;
  if (iring_full) {
    last_idx = (iring_head + IRINGBUF_SIZE - 1) % IRINGBUF_SIZE;
  } else {
    last_idx = iring_head - 1;
    if (last_idx < 0) last_idx += IRINGBUF_SIZE;
  }

  int start = iring_full ? iring_head : 0;

  for (int i = 0; i < count; i++) {
    int idx = (start + i) % IRINGBUF_SIZE;
    const ItraceRecord &rec = iringbuf[idx];

    uint32_t inst = rec.inst;
    // 按讲义示例，以“高位到低位”的顺序打印 4 个字节
    uint8_t b3 = (inst >> 24) & 0xff;
    uint8_t b2 = (inst >> 16) & 0xff;
    uint8_t b1 = (inst >>  8) & 0xff;
    uint8_t b0 = (inst >>  0) & 0xff;

    const char *mark = (idx == last_idx) ? "-->" : "   ";

    // 参考讲义排版：PC、反汇编、然后是 4 个字节
    printf("%s 0x%08x: %-32s  %02x %02x %02x %02x\n",
           mark,
           rec.pc,
           rec.disasm.c_str(),
           b3, b2, b1, b0);
  }
}

#else // NPC_ENABLE_ITRACE == 0

void itrace_init(NPCContext* ctx)                  { (void)ctx; }
void itrace_disassemble(uint64_t pc, uint32_t inst){ (void)pc; (void)inst; }
void itrace_step(NPCContext* ctx)                  { (void)ctx; }
void itrace_dump_iringbuf(void)                    {}

#endif