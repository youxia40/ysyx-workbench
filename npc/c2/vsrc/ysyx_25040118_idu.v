module ysyx_25040118_idu(            //指令译码单元
    input [31:0] ins,                                                         //ifu所得指令
    output wen,                                                  // 寄存器写使能
    output [4:0] rs1_addr,                                                         // 源寄存器地址
    output [4:0] rd_addr,                                                   // 目标寄存器地址
    output [31:0] imm,
    output ebreak_on                   //ebreak指令检测
);
    wire [2:0] funct3 = ins[14:12]; 
    wire [2:0] _unused_funct3 = funct3;                                         //避免警告
    
    assign rs1_addr  = ins[19:15];                                            // rs1
    assign rd_addr   = ins[11:7];                                                    // rd
    assign imm = {{20{ins[31]}}, ins[31:20]};                                       //扩展至32位

    wire [6:0] opcode = ins[6:0];
    assign wen = (opcode == 7'b0010011) ? 1'b1 : 1'b0;
    assign ebreak_on = (ins == 32'h00100073);                   //32'h00100073=32'b0000000_00001_00000_000_00000_1110011,ebreak
endmodule
