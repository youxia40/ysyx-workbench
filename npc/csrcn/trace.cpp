#include "trace.h"          // 自定义跟踪头文件
#include "sim_core.h"       // 包含SimContext定义
#include <cstdint>          // 固定宽度整数类型
#include <cstdio>           // 标准输入输出
#include <llvm/Support/raw_ostream.h>   // LLVM原始输出流（已通过Makefile路径配置)
#include <llvm/Support/Format.h>      // 包含 llvm::format_hex（关键头文件）

// -------------------------------
// 全局状态（示例，根据实际需求调整）
// -------------------------------
static bool trace_enabled = false;  // 跟踪是否启用
static FILE* trace_log = nullptr;  // 跟踪日志文件句柄（可选）
// -------------------------------
// 函数实现（与头文件声明完全一致）

/**
 * @brief 初始化跟踪系统
 * @param sim 仿真上下文指针
 */

bool init_trace(SimContext* sim) {
    (void)sim;  // 避免未使用参数警告
    
    FILE* trace_log = fopen("build/trace.log", "w");
    if (!trace_log) {
        fprintf(stderr, "Failed to open trace log file\n");
        return false; // 初始化失败
    }
    
    // 启用跟踪
    trace_enabled = true;
    // 示例：初始化LLVM输出流（若需要）
    llvm::outs().SetUnbuffered();  // 禁用缓冲，实时输出
    
    return true; // 初始化成功
}

bool is_trace_enabled() {
   return trace_enabled;
}

/**
 * @brief 关闭跟踪系统
 */
void close_trace() {
    // 示例：关闭日志文件
    if (trace_log) {
        fclose(trace_log);
        trace_log = nullptr;
    }
    
    // 示例：刷新LLVM输出流
    llvm::outs().flush();
}

/**
 * @brief 记录指令跟踪信息
 * @param pc 当前程序计数器（PC）
 * @param inst 当前指令编码
 */
void log_itrace(uint32_t pc, uint32_t inst) {
    if (!trace_enabled) return;
    
    llvm::raw_ostream& os = llvm::outs();
    // 修正：使用 llvm::format_hex 格式化十六进制
    os << "ITRACE: PC=0x" << llvm::format_hex(pc, /*width=*/0, /*upper=*/false)  // 小写十六进制
       << " | INST=0x" << llvm::format_hex(inst, /*width=*/0, /*upper=*/false)
       << " | Decoded: " << disassemble(pc, inst) << "\n";
    os.flush();
}

/**
 * @brief 记录函数调用跟踪信息
 * @param pc 调用点程序计数器（PC）
 * @param target_pc 目标函数入口PC
 */
void log_ftrace(uint32_t pc, uint32_t target_pc) {
    if (!trace_enabled) return;
    
    llvm::raw_ostream& os = llvm::outs();
    // 修正：使用 llvm::format_hex 格式化十六进制
    os << "FTRACE: CALL at PC=0x" << llvm::format_hex(pc, /*width=*/0, /*upper=*/false)  // 小写十六进制
       << " -> TARGET_PC=0x" << llvm::format_hex(target_pc, /*width=*/0, /*upper=*/false) << "\n";
    os.flush();
}

/**
 * @brief 记录内存访问跟踪信息
 * @param is_read 是否为读操作（true=读，false=写）
 * @param addr 访问的内存地址
 * @param len 访问的字节长度
 * @param data 访问的数据（仅写操作有效）
 */
void log_mtrace(bool is_read, uint32_t addr, int len, uint32_t data) {
    if (!trace_enabled) return;
    
    llvm::raw_ostream& os = llvm::outs();
    // 修正：使用 llvm::format_hex 格式化十六进制
    os << "MTRACE: " << (is_read ? "READ" : "WRITE") 
       << " | ADDR=0x" << llvm::format_hex(addr, /*width=*/0, /*upper=*/false)  // 小写十六进制
       << " | LEN=" << len 
       << " | DATA=0x" << llvm::format_hex(data, /*width=*/0, /*upper=*/false) << "\n";
    os.flush();
}

// -------------------------------
// 辅助函数（示例，根据实际需求添加）
// -------------------------------

/**
 * @brief 简单指令反汇编（示例，需根据实际架构实现）
 * @param pc 指令PC
 * @param inst 指令编码
 * @return 反汇编后的字符串
 */
const char* disassemble(uint32_t pc, uint32_t inst) {
    static char buffer[256];
    snprintf(buffer, sizeof(buffer), "0x%08x", inst);  // 默认返回十六进制
    return buffer;
}