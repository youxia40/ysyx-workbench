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
    input  [31:0] lsu_load_data,   // <<< 来自LSU的load结果
    output reg [31:0] next_pc,
    input         ebreak
);

    `ifndef SYNTHESIS
    import "DPI-C" function void npc_ebreak(input int pc);
    import "DPI-C" function void npc_ftrace_log(
        input longint unsigned pc,
        input longint unsigned target_pc,
        input int              is_call
    );
    `endif

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
    end

    //ftrace在jal/jalr时调用
    always @(*) begin
        if (!stop) begin
            if (is_jal) begin
                if (rd != 5'd0) begin
                    `ifndef SYNTHESIS
                    npc_ftrace_log({32'b0, pc},{32'b0, jal_target},1);
                    `endif
                end
            end
            else if (is_jalr) begin
                if (rd == 5'd0 && rs1 == 5'd1 && imm == 32'd0) begin
                    `ifndef SYNTHESIS
                    npc_ftrace_log({32'b0, pc},64'd0,0);
                    `endif
                end
                else if (rd != 5'd0) begin
                    `ifndef SYNTHESIS
                    npc_ftrace_log({32'b0, pc},{32'b0, jalr_target},1);
                    `endif
                end
            end
        end
    end

    wire [31:0] alu_op2 = is_alu_imm ? imm : src2;

    //result生成：load 直接用LSU输出，其它保持原样
    always @(*) begin
        if (is_load) begin
            result = lsu_load_data;     // <<< 关键修改：不再在EXU里扩展/读mem
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
                5'b10000: result = src1 - alu_op2;                           //sub
                5'b00001: result = src1 << alu_op2[4:0];                     //sll/slli
                5'b00101: result = src1 >> alu_op2[4:0];                     //srl/srli
                5'b00110: result = $signed(src1) >>> alu_op2[4:0];           //sra/srai
                5'b00010: result = ($signed(src1) <  $signed(alu_op2)) ? 32'd1 : 32'd0; //slt/slti
                5'b00011: result = (src1 < alu_op2) ? 32'd1 : 32'd0;         //sltu/sltiu
                5'b00100: result = src1 ^ alu_op2;                           //xor/xori
                5'b00111: result = src1 | alu_op2;                           //or/ori
                5'b01000: result = src1 & alu_op2;                           //and/andi
                5'b01010: result = pc + imm;                                 //auipc
                default:  result = 32'b0;
            endcase
        end
    end

    //ebreak通过DPI区分GOOD/BAD TRAP
    always @(posedge clk) begin
        if (ebreak && !stop) begin
            `ifndef SYNTHESIS
            npc_ebreak(pc);
            `endif
        end
    end

endmodule
