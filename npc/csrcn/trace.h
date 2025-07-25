#ifndef TRACE_H
#define TRACE_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include "sim_core.h"

// LLVM 头文件（使用完整的LLVM 14路径）
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/raw_os_ostream.h>
#include <llvm/Support/Casting.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/raw_ostream.h>

class Trace {
private:
    std::ofstream traceFile;
    bool enableDetailedTrace;
    int traceIndentLevel;
    std::map<uint64_t, std::string> addressToSymbol;

public:
    Trace(const std::string &traceFile = "trace.log");
    ~Trace();
    
    void setDetailedTrace(bool enable);
    void setSymbolMapping(uint64_t addr, const std::string &symbol);
    void log(const std::string &message);
    void logIndent();
    void logUnindent();
    void logInstruction(const llvm::Instruction &inst, 
                       const std::string &disassembly = "");
    void logFunctionEntry(const llvm::Function &F);
    void logFunctionExit(const llvm::Function &F);
};



bool init_trace(SimContext* sim); // 返回bool表示初始化结果
bool is_trace_enabled();          // 添加获取跟踪状态的函数

void close_trace();
void log_itrace(uint32_t pc, uint32_t inst);
void log_ftrace(uint32_t pc, uint32_t target_pc);
void log_mtrace(bool is_read, uint32_t addr, int len, uint32_t data);
const char* disassemble(uint32_t pc, uint32_t inst);

#endif // TRACE_H