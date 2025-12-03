module ysyx_25040118_idu (
    input         clk,
    input         rst,
    input         stop,
    input  [31:0] inst,
    input  [31:0] pc,
    output reg [4:0]  rd,
    output reg [4:0]  rs1,
    output reg [4:0]  rs2,
    output reg [31:0] imm,
    output reg [4:0]  alu_ctrl,
    output reg        ebreak,
    output reg        is_load,
    output reg        is_store,
    output reg        is_branch,
    output reg        is_jal,
    output reg        is_jalr,
    output reg        is_system,
    output reg        is_auipc,
    output reg        is_lui,
    output reg        is_alu_imm
);
    // RV32E: 寄存器索引只用到 x0~x15
    always @(*) begin
        rd  = inst[11:7]  & 5'b0_1111;
        rs1 = inst[19:15] & 5'b0_1111;
        rs2 = inst[24:20] & 5'b0_1111;
    end

    // 立即数生成
    always @(*) begin
        case (inst[6:0])
            // I-type
            7'b0010011,
            7'b0000011,
            7'b1100111: imm = {{20{inst[31]}}, inst[31:20]};
            // S-type
            7'b0100011: imm = {{20{inst[31]}}, inst[31:25], inst[11:7]};
            // B-type
            7'b1100011: imm = {{19{inst[31]}}, inst[31], inst[7],
                                inst[30:25], inst[11:8], 1'b0};
            // U-type
            7'b0110111,
            7'b0010111: imm = {inst[31:12], 12'b0};
            // J-type
            7'b1101111: imm = {{11{inst[31]}}, inst[31],
                                inst[19:12], inst[20],
                                inst[30:21], 1'b0};
            default:    imm = 32'b0;
        endcase
    end

    // 指令译码
    always @(*) begin
        // 默认值
        alu_ctrl   = 5'b00000;
        is_load    = 1'b0;
        is_store   = 1'b0;
        is_branch  = 1'b0;
        is_jal     = 1'b0;
        is_jalr    = 1'b0;
        is_system  = 1'b0;
        is_auipc   = 1'b0;
        is_lui     = 1'b0;
        is_alu_imm = 1'b0;
        ebreak     = 1'b0;

        casez (inst)
            // ===== 移位指令 =====
            32'b0000000_?????_?????_001_?????_0110011: alu_ctrl = 5'b00001; // sll
            32'b0000000_?????_?????_001_?????_0010011: begin                 // slli
                alu_ctrl   = 5'b00001;
                is_alu_imm = 1'b1;
            end

            32'b0000000_?????_?????_101_?????_0110011: alu_ctrl = 5'b00101; // srl
            32'b0000000_?????_?????_101_?????_0010011: begin                 // srli
                alu_ctrl   = 5'b00101;
                is_alu_imm = 1'b1;
            end

            32'b0100000_?????_?????_101_?????_0110011: alu_ctrl = 5'b00110; // sra
            32'b0100000_?????_?????_101_?????_0010011: begin                 // srai
                alu_ctrl   = 5'b00110;
                is_alu_imm = 1'b1;
            end

            // ===== 算术指令 =====
            32'b0000000_?????_?????_000_?????_0110011: alu_ctrl = 5'b00000; // add
            32'b???????_?????_?????_000_?????_0010011: begin                // addi
                alu_ctrl   = 5'b00000;
                is_alu_imm = 1'b1;
            end
            32'b0100000_?????_?????_000_?????_0110011: alu_ctrl = 5'b10000; // sub

            // auipc / lui
            32'b???????_?????_?????_???_?????_0010111: begin               // auipc
                alu_ctrl  = 5'b01010;
                is_auipc  = 1'b1;
            end
            32'b???????_?????_?????_???_?????_0110111: begin               // lui
                alu_ctrl  = 5'b01001;
                is_lui    = 1'b1;
            end

            // ===== 逻辑指令 =====
            32'b0000000_?????_?????_100_?????_0110011: alu_ctrl = 5'b00100; // xor
            32'b???????_?????_?????_100_?????_0010011: begin                // xori
                alu_ctrl   = 5'b00100;
                is_alu_imm = 1'b1;
            end

            32'b0000000_?????_?????_110_?????_0110011: alu_ctrl = 5'b00111; // or
            32'b???????_?????_?????_110_?????_0010011: begin                // ori
                alu_ctrl   = 5'b00111;
                is_alu_imm = 1'b1;
            end

            32'b0000000_?????_?????_111_?????_0110011: alu_ctrl = 5'b01000; // and
            32'b???????_?????_?????_111_?????_0010011: begin                // andi
                alu_ctrl   = 5'b01000;
                is_alu_imm = 1'b1;
            end

            // ===== 比较-置位指令 =====
            32'b0000000_?????_?????_010_?????_0110011: alu_ctrl = 5'b00010; // slt
            32'b???????_?????_?????_010_?????_0010011: begin                // slti
                alu_ctrl   = 5'b00010;
                is_alu_imm = 1'b1;
            end

            32'b0000000_?????_?????_011_?????_0110011: alu_ctrl = 5'b00011; // sltu
            32'b???????_?????_?????_011_?????_0010011: begin                // sltiu
                alu_ctrl   = 5'b00011;
                is_alu_imm = 1'b1;
            end

            // ===== 分支指令 =====
            32'b???????_?????_?????_000_?????_1100011: begin // beq
                is_branch = 1'b1;
                alu_ctrl  = 5'b01001;
            end
            32'b???????_?????_?????_001_?????_1100011: begin // bne
                is_branch = 1'b1;
                alu_ctrl  = 5'b01010;
            end
            32'b???????_?????_?????_100_?????_1100011: begin // blt
                is_branch = 1'b1;
                alu_ctrl  = 5'b01011;
            end
            32'b???????_?????_?????_101_?????_1100011: begin // bge
                is_branch = 1'b1;
                alu_ctrl  = 5'b01100;
            end
            32'b???????_?????_?????_110_?????_1100011: begin // bltu
                is_branch = 1'b1;
                alu_ctrl  = 5'b01101;
            end
            32'b???????_?????_?????_111_?????_1100011: begin // bgeu
                is_branch = 1'b1;
                alu_ctrl  = 5'b01110;
            end

            // ===== 跳转并链接 =====
            32'b???????_?????_?????_???_?????_1101111: begin // jal
                is_jal   = 1'b1;
                alu_ctrl = 5'b01111;
            end
            32'b???????_?????_?????_000_?????_1100111: begin // jalr
                is_jalr  = 1'b1;
                alu_ctrl = 5'b01111;
            end

            // ===== fence / fence.i =====
            32'b0000???_?????_00000_000_00000_0001111: ; // fence -> NOP
            32'b0000000_00000_00000_001_00000_0001111: ; // fence.i -> NOP

            // ===== 系统指令 ecall / ebreak =====
            // ecall: 当成普通 system 指令，不触发 ebreak
            32'b0000000_00000_00000_000_00000_1110011: begin
                is_system = 1'b1;
                ebreak    = 1'b0;
            end

            // ebreak: 用作 AM 的 halt
            32'b0000000_00001_00000_000_00000_1110011: begin
                is_system = 1'b1;
                ebreak    = 1'b1;
            end

            // ===== Load 指令 =====
            32'b???????_?????_?????_000_?????_0000011: begin // lb
                is_load  = 1'b1;
                alu_ctrl = 5'b00000;
            end
            32'b???????_?????_?????_001_?????_0000011: begin // lh
                is_load  = 1'b1;
                alu_ctrl = 5'b00000;
            end
            32'b???????_?????_?????_010_?????_0000011: begin // lw
                is_load  = 1'b1;
                alu_ctrl = 5'b00000;
            end
            32'b???????_?????_?????_100_?????_0000011: begin // lbu
                is_load  = 1'b1;
                alu_ctrl = 5'b00000;
            end
            32'b???????_?????_?????_101_?????_0000011: begin // lhu
                is_load  = 1'b1;
                alu_ctrl = 5'b00000;
            end

            // ===== Store 指令 =====
            32'b???????_?????_?????_000_?????_0100011: begin // sb
                is_store = 1'b1;
                alu_ctrl = 5'b00000;
            end
            32'b???????_?????_?????_001_?????_0100011: begin // sh
                is_store = 1'b1;
                alu_ctrl = 5'b00000;
            end
            32'b???????_?????_?????_010_?????_0100011: begin // sw
                is_store = 1'b1;
                alu_ctrl = 5'b00000;
            end

            //未实现指令
            default: begin
                is_system = 1'b1;
                ebreak    = 1'b1; //触发 BAD TRAP
            end
        endcase

        // debug 输出（当前全注释）
        if (!stop && inst !== 32'h00000013) begin
            // $display("[IDU] PC=0x%08x, INST=0x%08x", pc, inst);
        end
    end
endmodule
