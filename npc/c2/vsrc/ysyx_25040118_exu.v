module ysyx_25040118_exu(                                                    //执行单元
    input [31:0] rs1, 
    input [31:0] imm,
    output [31:0] outdata
);

    assign outdata = rs1 + imm;                                          //执行加法
endmodule
