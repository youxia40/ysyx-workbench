module ysyx_25040118_regfile (
    input         clk,
    input         rst,
    input         stop,
    input  [4:0]  waddr,
    input  [31:0] wdata,
    input         wen,
    input  [4:0]  raddr1,
    output [31:0] rdata1,
    input  [4:0]  raddr2,
    output [31:0] rdata2
);
    // RTL -> C：在每次写寄存器时把寄存器值同步到 npc_ctx.debug.regs 中
    import "DPI-C" function void npc_set_reg(input int idx, input int value);

    logic [31:0] rf [0:15];

    // 写操作
    always @(posedge clk) begin
        if (rst) begin
            for (integer i = 0; i < 16; i = i + 1) begin
                rf[i] <= 32'b0;
            end
        end
        else if (wen && waddr != 0) begin
            if (waddr < 16) begin
                rf[waddr] <= wdata;

                // 同步到调试上下文（给 sdb/expr/difftest 和 GOOD/BAD TRAP 用）
                npc_set_reg(waddr, wdata);

                // 只在非停止状态和非零值时输出调试信息
                if (!stop && wdata != 0) begin
                    //$display("[REGFILE] Write: x%0d = 0x%08x", waddr, wdata);
                end
            end
        end
    end

    // 读操作
    assign rdata1 = (raddr1 == 0) ? 32'b0 :
                    (raddr1 < 16) ? rf[raddr1] : 32'b0;

    assign rdata2 = (raddr2 == 0) ? 32'b0 :
                    (raddr2 < 16) ? rf[raddr2] : 32'b0;

endmodule
