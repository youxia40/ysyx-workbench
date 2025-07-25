module ysyx_25040118_idu (
    input  [31:0] inst,   // 输入指令
    input  [31:0] pc,     // 输入PC
    output [3:0]  rd,     // 目标寄存器（rd）[0-15]
    output [3:0]  rs1,    // 源寄存器1（rs1）[0-15]
    output [3:0]  rs2,    // 源寄存器2（rs2）[0-15]
    output [31:0] imm,    // 立即数
    output [6:0]  opcode, // 操作码
    output        ebreak, // EBREAK标志
    output reg [4:0] alu_ctrl // ALU控制信号
);

    // -------------------------------
    // 基本字段提取 (RV32E)
    // -------------------------------
    assign opcode = inst[6:0];
    assign rd = inst[11:7] & 4'hF;  // 限制在0-15范围
    assign rs1 = inst[19:15] & 4'hF; // 限制在0-15范围
    assign rs2 = inst[24:20] & 4'hF; // 限制在0-15范围

    // -------------------------------
    // 立即数生成 (RV32E)
    // -------------------------------
    wire [31:0] i_imm = {{20{inst[31]}}, inst[31:20]}; // I-type
    wire [31:0] s_imm = {{20{inst[31]}}, inst[31:25], inst[11:7]}; // S-type
    wire [31:0] b_imm = {{19{inst[31]}}, inst[7], inst[30:25], inst[11:8], 1'b0}; // B-type
    wire [31:0] u_imm = {inst[31:12], 12'b0}; // U-type
    wire [31:0] j_imm = {{12{inst[31]}}, inst[19:12], inst[20], inst[30:21], 1'b0}; // J-type
    
    // 多路选择立即数
    assign imm = 
        (opcode == 7'b0010011 || opcode == 7'b0000011 || opcode == 7'b1100111) ? i_imm : // I-type
        (opcode == 7'b0100011) ? s_imm : // S-type
        (opcode == 7'b1100011) ? b_imm : // B-type
        (opcode == 7'b0110111 || opcode == 7'b0010111) ? u_imm : // U-type
        (opcode == 7'b1101111) ? j_imm : // J-type
        32'h00000000;

    // -------------------------------
    // ALU控制信号生成 (RV32E) - 修复警告
    // -------------------------------
    always @(*) begin
        case (opcode)
            // 算术/逻辑运算
            7'b0010011: begin // I-type
                case (inst[14:12])
                    3'b000: alu_ctrl = 5'b00000; // ADDI
                    3'b001: alu_ctrl = 5'b00001; // SLLI
                    3'b010: alu_ctrl = 5'b00010; // SLTI
                    3'b011: alu_ctrl = 5'b00011; // SLTIU
                    3'b100: alu_ctrl = 5'b00100; // XORI
                    3'b101: alu_ctrl = (inst[30] ? 5'b00101 : 5'b00110); // SRLI/SRAI
                    3'b110: alu_ctrl = 5'b00111; // ORI
                    3'b111: alu_ctrl = 5'b01000; // ANDI
                    default: alu_ctrl = 5'b00000; // 添加默认分支
                endcase
            end
            
            // R-type
            7'b0110011: begin
                case ({inst[30], inst[14:12]})
                    {1'b0, 3'b000}: alu_ctrl = 5'b00000; // ADD
                    {1'b1, 3'b000}: alu_ctrl = 5'b00001; // SUB
                    {1'b0, 3'b001}: alu_ctrl = 5'b00010; // SLL
                    {1'b0, 3'b010}: alu_ctrl = 5'b00011; // SLT
                    {1'b0, 3'b011}: alu_ctrl = 5'b00100; // SLTU
                    {1'b0, 3'b100}: alu_ctrl = 5'b00101; // XOR
                    {1'b0, 3'b101}: alu_ctrl = 5'b00110; // SRL
                    {1'b1, 3'b101}: alu_ctrl = 5'b00111; // SRA
                    {1'b0, 3'b110}: alu_ctrl = 5'b01000; // OR
                    {1'b0, 3'b111}: alu_ctrl = 5'b01001; // AND
                    default: alu_ctrl = 5'b00000; // 添加默认分支
                endcase
            end
            
            // 其他指令
            default: alu_ctrl = 5'b00000; // 默认ADD
        endcase
    end

    // -------------------------------
    // EBREAK检测 (RV32E)
    // -------------------------------
    assign ebreak = (inst == 32'h00100073); // EBREAK指令编码

endmodule
