module ysyx_25040118_ifu (
    input         clk,
    input         rst,
    input         stop,
    input  [31:0] next_pc,
    output reg [31:0] pc,
    output [31:0] inst
);
    parameter VIRT_MEM_BASE = 32'h80000000;
    parameter PHYS_MEM_SIZE = 32'h08000000;

    import "DPI-C" function int npc_pmem_read(input int raddr);

    reg [31:0] pc_reg;
    wire [31:0] phys_addr  = pc_reg - VIRT_MEM_BASE;
    wire        valid_addr = (phys_addr < PHYS_MEM_SIZE);

    always @(posedge clk) begin
        if (rst) begin
            pc_reg <= VIRT_MEM_BASE;
        end
        else if (!stop) begin
            pc_reg <= next_pc;
        end
    end

    assign pc   = pc_reg;
    assign inst = valid_addr ? npc_pmem_read(phys_addr) : 32'h00000013;     //NOP

    always @(posedge clk) begin
        if (!rst && !stop && inst !== 32'h00000013) begin


            //$display("[IFU] PC=0x%08x, INST=0x%08x", pc_reg, inst);
        end
    end
endmodule
