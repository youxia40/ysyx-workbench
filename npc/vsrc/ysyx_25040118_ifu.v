module ysyx_25040118_ifu (
    input clk,
    input rst,
    input stop,
    input [31:0] next_pc,
    output [31:0] pc,
    output [31:0] inst
);
    parameter VIRT_MEM_BASE = 32'h80000000;//虚拟地址基址
    parameter PHYS_MEM_SIZE = 32'h08000000;//物理内存大小128MB

    `ifndef SYNTHESIS
    //仿真态通过DPI读取内存,综合态不使用该接口
    import "DPI-C" function int npc_pmem_read(input int raddr);
    `endif


    reg [31:0] pc_reg;//当前取指PC寄存器
    wire [31:0] phys_addr  = pc_reg - VIRT_MEM_BASE;//虚拟地址映射到物理偏移
    wire valid_addr = (phys_addr < PHYS_MEM_SIZE);//地址合法性检查

    //PC时序更新:复位回到基址,运行态跟随next_pc
    always @(posedge clk) begin
        if (rst) begin
            pc_reg <= VIRT_MEM_BASE;
        end
        else if (!stop) begin
            pc_reg <= next_pc;
        end
    end

    assign pc = pc_reg;//输出当前PC值

    //地址合法时通过DPI接口读取内存,否则输出NOP指令(ADDI x0,x0,0)
    `ifndef SYNTHESIS
    //32'h00000013:将x0的值+0，zai写回x0。
    assign inst = valid_addr ? npc_pmem_read(phys_addr) : 32'h00000013;     //NOP
    `else
    assign inst = valid_addr ? 'h00000000 : 32'h00000013;     //NOP
    `endif
    
endmodule
