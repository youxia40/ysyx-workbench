module ysyx_25040118_exu (
    input         clk,
    input         rst,
    input         stop,
    input  [31:0] pc,
    input  [31:0] inst,
    input  [31:0] src1,
    input  [31:0] src2,
    input  [31:0] imm,
    input  [4:0]  alu_ctrl,
    input         is_load,
    input         is_store,
    input         is_branch,
    input         is_jal,
    input         is_jalr,
    input         is_system,
    input         is_auipc,
    input         is_lui,
    input         is_alu_imm,
    output reg [31:0] result,
    output reg [31:0] next_pc,
    input         ebreak
);
    parameter VIRT_MEM_BASE = 32'h80000000;

    import "DPI-C" function int  npc_pmem_read (input int raddr);
    import "DPI-C" function void npc_pmem_write(input int waddr, input int wdata, input byte wmask);
    import "DPI-C" function void npc_ebreak    (input int pc);
    //ftrace相关
    import "DPI-C" function void npc_ftrace_log(
        input longint unsigned pc,
        input longint unsigned target_pc,
        input int              is_call
    );

    //提前解码rd/rs1，方便判断call / ret
    wire [4:0] rd  = inst[11:7];
    wire [4:0] rs1 = inst[19:15];

    //jal目标地址
    wire [31:0] jal_target   = pc + imm;
    wire [31:0] jalr_target  = (src1 + imm) & ~32'h1;

    //PC更新
    always @(*) begin
        next_pc = pc + 4;

        if (is_jal) begin
            next_pc = jal_target;
        end
        else if (is_jalr) begin
            next_pc = jalr_target;
        end
        else if (is_branch) begin
            case (inst[14:12])
                3'b000: next_pc = (src1 == src2)                   ? (pc + imm) : (pc + 4); // beq
                3'b001: next_pc = (src1 != src2)                   ? (pc + imm) : (pc + 4); // bne
                3'b100: next_pc = ($signed(src1) < $signed(src2))  ? (pc + imm) : (pc + 4); // blt
                3'b101: next_pc = ($signed(src1) >= $signed(src2)) ? (pc + imm) : (pc + 4); // bge
                3'b110: next_pc = (src1 < src2)                    ? (pc + imm) : (pc + 4); // bltu
                3'b111: next_pc = (src1 >= src2)                   ? (pc + imm) : (pc + 4); // bgeu
                default: next_pc = pc + 4;
            endcase
        end

        if (!stop && (is_jal || is_jalr || is_branch)) begin
            
            //$display("[EXU] Jump/Branch: PC=0x%08x -> 0x%08x", pc, next_pc);
        end
    end


    //ftrace在jal时调用
    always @(*) begin
        if (stop) begin

            //停机不记录
        end
        else begin


            //识别CALL和RET指令
            //jal  rd, off      且 rd != x0                     ->CALL
            //jalr rd, rs1, 0   且 rd==x0 && rs1==x1            ->RET
            //jalr rd, rs1, imm 且 rd!=x0                       ->CALL

            if (is_jal) begin
                if (rd != 5'd0) begin
                    npc_ftrace_log({32'b0, pc},{32'b0, jal_target},1);  // is_call
                end
            end

            else if (is_jalr) begin
                if (rd == 5'd0 && rs1 == 5'd1 && imm == 32'd0) begin

                    //ret:jalr x0, x1, 0
                    npc_ftrace_log({32'b0, pc},64'd0,0);
                end

                else if (rd != 5'd0) begin

                    //其他jalr带 rd，当作call（函数指针等）
                    npc_ftrace_log({32'b0, pc},{32'b0, jalr_target},1);                   //CALL
                end
            end
        end
    end

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
                3'b000: mem_wmask = 4'b0001;            // sb
                3'b001: mem_wmask = 4'b0011;            // sh
                3'b010: mem_wmask = 4'b1111;                // sw
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
            mem_rdata = npc_pmem_read(phys_addr);
        end else begin
            mem_rdata = 32'b0;
        end
    end

    //在时钟上升沿进行存储
    always @(posedge clk) begin
        if (mem_we) begin
            npc_pmem_write(phys_addr, mem_wdata, mem_wmask);
            if (!stop) begin

                //$display("[MEM] Write: VADDR=0x%08x DATA=0x%08x", virt_addr, mem_wdata);
            end
        end
    end



    wire [31:0] alu_op2 = is_alu_imm ? imm : src2;

    always @(*) begin
        if (is_load) begin

            case (inst[14:12])
                3'b000: result = $signed(mem_rdata[7:0]);    // lb
                3'b001: result = $signed(mem_rdata[15:0]);  // lh
                3'b010: result = mem_rdata;                   // lw
                3'b100: result = mem_rdata[7:0];            // lbu
                3'b101: result = mem_rdata[15:0];           // lhu
                default: result = 32'b0;
            endcase
        end
        else if (is_auipc) begin
            result = pc + imm;
        end
        else if (is_lui) begin
            result = imm;
        end
        else if (is_jal || is_jalr) begin
            result = pc + 4;
        end
        else begin
            case (alu_ctrl)
                5'b00000: result = src1 + alu_op2;                           //add/addi
                5'b10000: result = src1 - alu_op2;                          //sub
                5'b00001: result = src1 << alu_op2[4:0];                     //sll/slli
                5'b00101: result = src1 >> alu_op2[4:0];                     //srl/srli
                5'b00110: result = $signed(src1) >>> alu_op2[4:0];          //sra/srai
                5'b00010: result = ($signed(src1) <  $signed(alu_op2)) ? 32'd1 : 32'd0; //slt/slti
                5'b00011: result = (src1 < alu_op2) ? 32'd1 : 32'd0;         //sltu/sltiu
                5'b00100: result = src1 ^ alu_op2;                              //xor/xori
                5'b00111: result = src1 | alu_op2;                          //or/ori
                5'b01000: result = src1 & alu_op2;                              //and/andi
                5'b01010: result = pc + imm;                                 //auipc
                default: result = 32'b0;
            endcase
        end

        if (!stop && !is_load && !is_store) begin
            //$display("[EXU] ALU: PC=0x%08x, RESULT=0x%08x", pc, result);
        end
    end



    //ebreak通过DPI区分GOOD/BAD TRAP
    always @(posedge clk) begin
        if (ebreak && !stop) begin
            npc_ebreak(pc);
            //$display("[EXU] EBREAK at PC=0x%08x", pc);
        end
    end

    always @(*) begin
        if (is_load && !stop) begin
            //$display("[MEM] Read : VADDR=0x%08x DATA=0x%08x", virt_addr, mem_rdata);
        end
    end
endmodule
