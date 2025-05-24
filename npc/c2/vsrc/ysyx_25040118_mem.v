module ysyx_25040118_mem(
    input [31:0] addr,
    output [31:0] ins
);

    wire [21:0] addr_high_unused = addr[31:10];  // 虚拟信号
    /* verilator lint_off UNUSED */
    wire [21:0] addr_high_unused_connected = addr_high_unused;                  //为了连接虚拟信号以避免警告

    reg [31:0] mem[0:1023];
    initial begin
        mem[0] = 32'b0000000_01011_00000_000_00001_0010011;             //addi格式：imm rs1 000 rd 0010011  addi x0,x0,11
        mem[1] = 32'b0000000_01011_00001_000_00001_0010011; 
        mem[2] = 32'b0000000_01011_00001_000_00001_0010011;
        mem[3] = 32'b0000000_00001_00000_000_00000_1110011;              //ebreak指令：0000000 rs1 000 rd 1110011   
        
        mem[4] = 32'b0000000_01011_00001_000_00001_0010011; 
    end
    wire [31:0] offset = addr - 32'h80000000;
    wire [9:0] mem_index = offset[11:2];  // 等效于右移2位后取低10位

    assign ins = mem[mem_index];                                              //读取指令 
endmodule
