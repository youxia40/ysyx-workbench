module ysyx_25040118_Registers (
    input         clk,      // 时钟
    input         rst,      // 复位（低有效）
    input  [3:0]  waddr,    // 写寄存器地址（0-15）
    input  [31:0] wdata,    // 写寄存器数据
    input         wren,     // 写使能
    input  [3:0]  raddr1,   // 读寄存器1地址
    output [31:0] rdata1,   // 读寄存器1数据
    input  [3:0]  raddr2,   // 读寄存器2地址
    output [31:0] rdata2    // 读寄存器2数据
);

    // -------------------------------
    // 寄存器文件 (16x32位, RV32E)
    // -------------------------------
    reg [31:0] reg_file [0:15]; // 16个寄存器（0号寄存器始终为0）

    // 寄存器写
    always @(posedge clk) begin
        if (rst) begin
            // 复位所有寄存器为0（除了0号）
            for (int i = 1; i < 16; i = i + 1) begin
                reg_file[i] <= 32'h00000000;
            end
        end else if (wren && waddr != 0) begin // 0号寄存器不可写
            reg_file[waddr] <= wdata;
        end
    end

    // 寄存器读
    assign rdata1 = (raddr1 == 0) ? 32'h00000000 : reg_file[raddr1];
    assign rdata2 = (raddr2 == 0) ? 32'h00000000 : reg_file[raddr2];

    // -------------------------------
    // 调试输出
    // -------------------------------
    always @(posedge clk) begin
        if (rst) begin
            $display("[REG] Reset all registers");
        end else if (wren && waddr != 0) begin
            $display("[REG] Write: x%1d = 0x%08x", waddr, wdata);
        end
    end

endmodule
