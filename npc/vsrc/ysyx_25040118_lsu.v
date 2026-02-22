module ysyx_25040118_lsu (//访存单元,处理Load/Store并与DPI内存接口交互
    input clk,
    input rst,
    input stop,
    input [31:0] inst,
    input [31:0] src1,
    input [31:0] src2,
    input [31:0] imm,
    input is_load,
    input is_store,
    output reg [31:0] load_data
);
    parameter VIRT_MEM_BASE = 32'h80000000;

`ifndef SYNTHESIS
    import "DPI-C" function int  npc_pmem_read (input int raddr);
    import "DPI-C" function void npc_pmem_write(input int waddr, input int wdata, input byte wmask);
`endif

    reg        mem_we;
    reg [31:0] virt_addr;
    reg [31:0] mem_wdata;
    reg [3:0]  mem_wmask;
    reg [31:0] mem_rdata;

    wire [1:0] lane         = virt_addr[1:0];//字节通道选择
    wire [31:0] aligned_v   = {virt_addr[31:2], 2'b00};//按word对齐地址
    wire [31:0] phys_addr   = aligned_v - VIRT_MEM_BASE;//转换成DPI偏移地址


    always @(*) begin
        mem_we    = 1'b0;
        virt_addr = 32'b0;
        mem_wdata = 32'b0;
        mem_wmask = 4'b0000;

        if (is_store) begin//Store阶段生成写掩码和写数据
            mem_we    = 1'b1;
            virt_addr = src1 + imm;

            case (inst[14:12])
                3'b000: begin //sb
                    mem_wmask = (4'b0001 << lane);
                    mem_wdata = {24'b0, src2[7:0]} << (8*lane);
                end
                3'b001: begin //sh
                    mem_wmask = (4'b0011 << lane);
                    mem_wdata = {16'b0, src2[15:0]} << (8*lane);
                end
                3'b010: begin //sw
                    mem_wmask = 4'b1111;
                    mem_wdata = src2;
                end
                default: begin
                    mem_wmask = 4'b0000;
                    mem_wdata = 32'b0;
                end
            endcase
        end
        else if (is_load) begin//Load阶段仅需要形成目标地址
            virt_addr = src1 + imm;
        end
    end

    //读路径:load在组合逻辑直接读取DPI返回值
    always @(*) begin
        if (is_load) begin//Load在组合逻辑里直接读DPI
`ifndef SYNTHESIS
            mem_rdata = npc_pmem_read(phys_addr);
`else
            mem_rdata = 32'b0;
`endif
        end else begin
            mem_rdata = 32'b0;
        end
    end

    //写路径:store在时钟上升沿提交写请求
    always @(posedge clk) begin
        if (mem_we && !stop) begin//Store在时钟沿提交写入
`ifndef SYNTHESIS
            npc_pmem_write(phys_addr, mem_wdata, mem_wmask);
`endif
        end
    end


    wire [31:0] shifted = mem_rdata >> (8*lane);//把目标字节移到最低位便于扩展

    always @(*) begin
        if (is_load) begin//按funct3做符号扩展或零扩展
            case (inst[14:12])
                3'b000: load_data = {{24{shifted[7]}},  shifted[7:0]};   //lb
                3'b001: load_data = {{16{shifted[15]}}, shifted[15:0]};  //lh
                3'b010: load_data = mem_rdata;                            //lw,地址对齐由软件保证
                3'b100: load_data = {24'b0, shifted[7:0]};                //lbu
                3'b101: load_data = {16'b0, shifted[15:0]};               //lhu
                default: load_data = 32'b0;
            endcase
        end else begin
            load_data = 32'b0;
        end
    end

endmodule
