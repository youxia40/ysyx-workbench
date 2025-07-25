module ysyx_25040118_ifu (
    input         clk,      // 时钟
    input         rst,      // 复位（低有效）
    input  [31:0] next_pc,  // 输入下一条PC（来自EXU）
    output reg [31:0] pc,   // 输出当前PC（到IDU）
    output [31:0] inst      // 输出指令（到IDU）
);

    // -------------------------------
    // 指令内存访问 (RV32E)
    // -------------------------------
    import "DPI-C" function int pmem_read(input int raddr);
    
    // 指令读取
    assign inst = pmem_read(pc);

    // -------------------------------
    // PC更新逻辑 (RV32E)
    // -------------------------------
    always @(posedge clk) begin
        if (rst) begin
            pc <= 32'h80000000; // 复位PC（RV32E起始地址）
        end else begin
            pc <= next_pc;      // 更新PC
        end
    end

    // -------------------------------
    // 调试输出
    // -------------------------------
    always @(posedge clk) begin
        if (rst) begin
            $display("[IFU] Reset, PC=0x80000000");
        end else begin
            $display("[IFU] PC=0x%08x, INST=0x%08x", pc, inst);
        end
    end

endmodule
