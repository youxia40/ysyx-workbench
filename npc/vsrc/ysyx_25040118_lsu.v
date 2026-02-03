module ysyx_25040118_lsu (//处理load/store指令的地址计算、内存读写等操作
    input         clk,
    input         rst,
    input         stop,
    input  [31:0] inst,
    input  [31:0] src1,
    input  [31:0] src2,
    input  [31:0] imm,
    input         is_load,
    input         is_store,
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

    wire [31:0] phys_addr = virt_addr - VIRT_MEM_BASE;

    //生成l/s地址、写数据和写掩码
    always @(*) begin
        mem_we    = 1'b0;
        virt_addr = 32'b0;
        mem_wdata = 32'b0;
        mem_wmask = 4'b0000;

        if (is_store) begin
            mem_we    = 1'b1;
            virt_addr = src1 + imm;
            mem_wdata = src2;

            case (inst[14:12])
                3'b000: mem_wmask = 4'b0001; //sb
                3'b001: mem_wmask = 4'b0011; //sh
                3'b010: mem_wmask = 4'b1111; //sw
                default: mem_wmask = 4'b0000;
            endcase
        end
        else if (is_load) begin
            virt_addr = src1 + imm;
        end
    end

    //只在load时触发物理内存读
    always @(*) begin
        if (is_load) begin
            `ifndef SYNTHESIS
            mem_rdata = npc_pmem_read(phys_addr);
            `else
            mem_rdata = 32'b0;
            `endif
        end else begin
            mem_rdata = 32'b0;
        end
    end

    //在时钟上升沿进行存储
    always @(posedge clk) begin
        if (mem_we) begin
            `ifndef SYNTHESIS
            npc_pmem_write(phys_addr, mem_wdata, mem_wmask);
            `endif
        end
    end

    //load符号/零扩展
    always @(*) begin
        if (is_load) begin
            case (inst[14:12])
                3'b000: load_data = $signed(mem_rdata[7:0]);    //lb
                3'b001: load_data = $signed(mem_rdata[15:0]);   //lh
                3'b010: load_data = mem_rdata;                  //lw
                3'b100: load_data = mem_rdata[7:0];             //lbu
                3'b101: load_data = mem_rdata[15:0];            //lhu
                default: load_data = 32'b0;
            endcase
        end else begin
            load_data = 32'b0;
        end
    end

endmodule
