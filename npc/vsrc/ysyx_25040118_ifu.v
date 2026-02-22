module ysyx_25040118_ifu (//取指单元,根据PC读取指令并输出给译码级
    input clk,
    input rst,
    input stop,
    input [31:0] next_pc,
    output [31:0] pc,
    output [31:0] inst
);
    parameter VIRT_MEM_BASE = 32'h80000000;//虚拟地址基址
    parameter PHYS_MEM_SIZE = 32'h08000000;//可访问物理空间大小

    `ifndef SYNTHESIS
    //仿真态通过DPI读取内存,综合态不使用该接口
    import "DPI-C" function int npc_pmem_read(input int raddr);
    `endif


    reg [31:0] pc_reg;//当前取指PC寄存器
    wire [31:0] phys_addr  = pc_reg - VIRT_MEM_BASE;//虚拟地址映射到物理偏移
    wire        valid_addr = (phys_addr < PHYS_MEM_SIZE);//地址合法性检查

    //PC时序更新:复位回到基址,运行态跟随next_pc
    always @(posedge clk) begin
        if (rst) begin
            pc_reg <= VIRT_MEM_BASE;
        end
        else if (!stop) begin
            pc_reg <= next_pc;
        end
    end

    assign pc   = pc_reg;//导出当前PC给后级与调试端口

    //取指策略:地址合法则读内存,否则注入NOP防止异常扩散
    `ifndef SYNTHESIS
    assign inst = valid_addr ? npc_pmem_read(phys_addr) : 32'h00000013;     //NOP
    `else
    assign inst = valid_addr ? 'h00000000 : 32'h00000013;     //NOP
    `endif


    //预留调试插桩位置,当前不影响功能
    always @(posedge clk) begin
        if (!rst && !stop && inst !== 32'h00000013) begin



        end
    end
endmodule
