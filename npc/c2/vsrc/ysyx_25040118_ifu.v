module ysyx_25040118_ifu(                                                   //指令获取单元
    input clk,
    input reset,
    input [31:0] next_pc,                                                     //下一个pc
    output reg [31:0] pc                                                //当前pc
);
    always @(posedge clk) begin
        if (reset) begin
            pc <= 32'h8000_0000;                                          //复位时pc=0x8000_0000
        end 
        else begin
            pc <= next_pc;                                                 //更新pc
        end
    end
endmodule
