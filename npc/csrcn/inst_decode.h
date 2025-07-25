#ifndef INST_DECODE_H
#define INST_DECODE_H

#include <cstdint>
#include <cstring>
#include <string>
#include <memory>  // 用于std::unique_ptr（可选）

// -------------------------------
// 兼容性配置：根据是否安装LLVM启用功能
// -------------------------------
// 若已安装LLVM（路径正确），则启用反汇编功能
// 否则仅保留基础结构（不影响其他功能）
#ifdef ENABLE_LLVM_DISASSEMBLER

// -------------------------------
// 关键修复：动态获取LLVM头文件路径（兼容不同安装方式）
// -------------------------------
// 尝试自动检测LLVM路径（适用于Linux常见安装方式）
#if defined(__linux__)
#  ifndef LLVM_INCLUDE_PATH
#    define LLVM_INCLUDE_PATH "/usr/include/llvm-14"  // 默认路径（可根据实际修改）
#  endif
#elif defined(__APPLE__)
#  ifndef LLVM_INCLUDE_PATH
#    define LLVM_INCLUDE_PATH "/usr/local/opt/llvm/include"  // macOS Homebrew安装路径
#  endif
#endif

// 若未自动检测到路径，强制用户配置（否则编译失败）
#ifndef LLVM_INCLUDE_PATH
#  error "LLVM头文件路径未配置！请设置LLVM_INCLUDE_PATH宏（如：-DLLVM_INCLUDE_PATH=/path/to/llvm/include）"
#endif

// 包含LLVM核心头文件（按依赖顺序排列）
#include <llvm/Support/raw_ostream.h>    // 基础IO支持（必须优先包含）
#include <llvm/MC/MCDisassembler.h>      // 反汇编器核心类
#include <llvm/MC/MCSubtargetInfo.h>     // 子目标信息（反汇编依赖）
#include <llvm/MC/MCContext.h>           // 上下文管理（可选，根据功能需求）
#include <llvm/IR/Instructions.h>        // 指令类型定义（可选）

#endif // ENABLE_LLVM_DISASSEMBLER

// -------------------------------
// 指令解码结果结构体（与LLVM解耦）
// -------------------------------
enum InstType {
    TYPE_R, TYPE_I, TYPE_S, TYPE_B, TYPE_U, TYPE_J, TYPE_UNKNOWN
};

struct InstInfo {
    uint32_t pc;          // 程序计数器
    uint32_t inst;        // 指令机器码（十六进制）
    InstType type;        // 指令类型
    char name[16];        // 指令名称（如"add"）
    char op_str[64];      // 操作数字符串（如"x1, x2, x3"）
    std::string llvm_asm; // LLVM反汇编结果（仅当ENABLE_LLVM_DISASSEMBLER时有效）
};

// -------------------------------
// LLVM反汇编上下文（仅当LLVM可用时定义）
// -------------------------------
#ifdef ENABLE_LLVM_DISASSEMBLER
struct LLVMDisassemblerContext {
    std::unique_ptr<const llvm::MCDisassembler> disassembler;  // 自动管理内存
    const llvm::MCSubtargetInfo* subtarget;                    // 子目标信息指针
    llvm::MCContext* context;                                  // MC上下文（可选）
};
#endif

// -------------------------------
// 核心函数声明（与LLVM解耦）
// -------------------------------
void decode_inst(InstInfo* info);  // 基础解码（必选）

#ifdef ENABLE_LLVM_DISASSEMBLER
// LLVM增强版解码（可选，仅在LLVM可用时生效）
void decode_inst_with_llvm(LLVMDisassemblerContext* ctx, InstInfo* info);
#endif

#endif // INST_DECODE_H