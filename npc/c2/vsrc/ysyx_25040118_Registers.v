module ysyx_25040118_Registers #(ADDR_WIDTH = 5, DATA_WIDTH = 32) (
    input clk,
    input [DATA_WIDTH-1:0] wdata,
    input [ADDR_WIDTH-1:0] waddr,
    input wen,                                              //写使能
    output [DATA_WIDTH-1:0] rdata1,                                 //rs1
    input [ADDR_WIDTH-1:0] raddr1
);
    reg [DATA_WIDTH-1:0] rf [2**ADDR_WIDTH-1:0];                                        //32个32位寄存器
    always @(posedge clk) begin
        if (waddr == 0) rf[waddr] <= 0;
        else
        if (wen) rf[waddr] <= wdata;
    end
    assign rdata1 = (raddr1 == 0) ? 0 : rf[raddr1];                                          //读寄存器
endmodule
