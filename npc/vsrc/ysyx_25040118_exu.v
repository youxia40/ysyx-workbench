module ysyx_25040118_exu (
    input clk,
    input rst,
    input stop,
    input [31:0] pc,
    input [31:0] inst,
    input [31:0] src1,
    input [31:0] src2,
    input [31:0] imm,
    input [4:0]alu_ctrl,
    input is_load,
    input is_store,
    input is_branch,
    input is_jal,
    input is_jalr,
    input is_system,

    input is_csrrw,
    input is_csrrs,
    input is_csrrc,
    input is_csrrwi,
    input is_csrrsi,
    input is_csrrci,
    input is_mret,
    input is_ecall,

    input is_auipc,
    input is_lui,
    input is_alu_imm,
    input [31:0] csr_rdata,
    input [31:0] csr_mtvec,
    input [31:0] csr_mepc,
    input [31:0] lsu_load_data,//LSU的load结果
    input ebreak, //来自IDU的ebreak信号,用于触发DPI调用
    output reg [31:0] result,
    output reg [31:0] next_pc,
    output [11:0] csr_addr,
    output reg [31:0] csr_wdata,
    output reg csr_we,
    output reg trap_we,
    output reg [31:0] trap_epc,
    output reg [31:0] trap_cause
);

    `ifndef SYNTHESIS//综合不使用
    import "DPI-C" function void npc_ebreak(input int pc);
    import "DPI-C" function void npc_ftrace_log(
        input longint unsigned pc,
        input longint unsigned target_pc,
        input int is_call
    );
    import "DPI-C" function void npc_etrace_trap(
        input int unsigned no,
        input int unsigned epc,
        input int unsigned handler
    );
    import "DPI-C" function void npc_etrace_mret(
        input int unsigned from,
        input int unsigned to
    );
    `endif

    //提前解码rd和rs1,后续用于识别call/ret模式
    wire [4:0] rd = inst[11:7];
    wire [4:0] rs1 = inst[19:15];
    wire [31:0] csr_uimm = {27'b0, inst[19:15]};
    wire is_csr_any = is_csrrw | is_csrrs | is_csrrc | is_csrrwi | is_csrrsi | is_csrrci;//是否是任意CSR指令
    assign csr_addr = inst[31:20];

    //跳转目标地址计算,jal为pc相对,jalr为寄存器相对并清除最低位
    wire [31:0] jal_target = pc + imm;
    wire [31:0] jalr_target = (src1 + imm) & ~32'h1;

    //下一条PC选择优先级:jal>jalr>branch>pc+4
    always @(*) begin
        next_pc = pc + 4;

        if (is_mret) begin
            next_pc = csr_mepc;
        end
        else if (is_ecall) begin
            next_pc = csr_mtvec;
        end
        else if (is_jal) begin
            next_pc = jal_target;
        end
        else if (is_jalr) begin
            next_pc = jalr_target;
        end
        else if (is_branch) begin
            case (inst[14:12])
                3'b000: next_pc = (src1 == src2) ? (pc + imm) : (pc + 4); //beq
                3'b001: next_pc = (src1 != src2) ? (pc + imm) : (pc + 4); //bne
                3'b100: next_pc = ($signed(src1) < $signed(src2)) ? (pc + imm) : (pc + 4); //blt
                3'b101: next_pc = ($signed(src1) >= $signed(src2)) ? (pc + imm) : (pc + 4); //bge
                3'b110: next_pc = (src1 < src2) ? (pc + imm) : (pc + 4); //bltu
                3'b111: next_pc = (src1 >= src2) ? (pc + imm) : (pc + 4); //bgeu
                default: next_pc = pc + 4;
            endcase
        end
    end

    //ftrace调用:
    //jal且rd!=x0视为调用
    //jalr且rd=x0,rs1=ra,imm=0视为返回
    //其余jalr且rd!=x0视为调用
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

    //ALU第二操作数由is_alu_imm决定,立即数指令走imm,寄存器指令走src2
    wire [31:0] alu_op2 = is_alu_imm ? imm : src2;

    //写回结果选择:
    //load由LSU提供已扩展数据
    //auipc/lui/jump走专用路径
    //其余由alu_ctrl驱动ALU运算
    always @(*) begin
        if (is_load) begin
            result = lsu_load_data;
        end
        else if (is_csr_any) begin
            result = csr_rdata;
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
            case (alu_ctrl)//编码来自IDU译码结果
                5'b00000: result = src1 + alu_op2;                           //add/addi
                5'b10000: result = src1 - alu_op2;                           //sub
                5'b00001: result = src1 << alu_op2[4:0];                     //sll/slli
                5'b00101: result = src1 >> alu_op2[4:0];                     //srl/srli
                5'b00110: result = $signed(src1) >>> alu_op2[4:0];           //sra/srai
                5'b00010: result = ($signed(src1) < $signed(alu_op2)) ? 32'd1 : 32'd0; //slt/slti
                5'b00011: result = (src1 < alu_op2) ? 32'd1 : 32'd0;         //sltu/sltiu
                5'b00100: result = src1 ^ alu_op2;                           //xor/xori
                5'b00111: result = src1 | alu_op2;                           //or/ori
                5'b01000: result = src1 & alu_op2;                           //and/andi
                5'b01010: result = pc + imm;                                 //auipc
                default:  result = 32'b0;
            endcase
        end
    end

    //CSR写口与trap侧带信息
    always @(*) begin
        csr_we = 1'b0;
        csr_wdata = 32'b0;
        trap_we = 1'b0;
        trap_epc = 32'b0;
        trap_cause = 32'b0;

        if (is_csrrw) begin
            csr_we = 1'b1;
            csr_wdata = src1;
        end
        else if (is_csrrs) begin
            if (rs1 != 5'd0) begin
                csr_we = 1'b1;
                csr_wdata = csr_rdata | src1;
            end
        end
        else if (is_csrrc) begin
            if (rs1 != 5'd0) begin
                csr_we = 1'b1;
                csr_wdata = csr_rdata & (~src1);
            end
        end
        else if (is_csrrwi) begin
            csr_we = 1'b1;
            csr_wdata = csr_uimm;
        end
        else if (is_csrrsi) begin
            if (inst[19:15] != 5'd0) begin
                csr_we = 1'b1;
                csr_wdata = csr_rdata | csr_uimm;
            end
        end
        else if (is_csrrci) begin
            if (inst[19:15] != 5'd0) begin
                csr_we = 1'b1;
                csr_wdata = csr_rdata & (~csr_uimm);
            end
        end

        if (is_ecall) begin
            trap_we = 1'b1;
            trap_epc = pc;
            trap_cause = 32'h0000000b;
        end
    end

    //ebreak在时钟上升沿提交到DPI,由a0值区分GOODTRAP或BADTRAP
    always @(posedge clk) begin
        if (!rst && !stop) begin
            if (is_ecall) begin
                `ifndef SYNTHESIS
                npc_etrace_trap(32'h0000000b, pc, csr_mtvec);
                `endif
            end
            else if (is_mret) begin
                `ifndef SYNTHESIS
                npc_etrace_mret(pc, csr_mepc);
                `endif
            end
        end

        if (!rst && ebreak && !stop) begin
            `ifndef SYNTHESIS
            npc_ebreak(pc);
            `endif
        end
    end

endmodule
