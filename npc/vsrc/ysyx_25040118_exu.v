module ysyx_25040118_exu (
    input         clk,      // 时钟
    input         rst,      // 复位（低有效）
    input  [31:0] pc,        // 输入PC（来自IFU）
    input  [31:0] inst,       // 输入指令（来自IFU）
    input  [31:0] src1,     // 源操作数1（来自寄存器文件）
    input  [31:0] src2,     // 源操作数2（来自寄存器文件）
    input  [31:0] imm,      // 立即数（来自IDU）
    input  [4:0]  alu_ctrl, // ALU控制信号（来自IDU）
    output [31:0] result,   // 计算结果
    output [31:0] next_pc,  // 下一条PC
    output        mem_wren, // 存储器写使能
    output [31:0] mem_addr, // 存储器地址
    output [31:0] mem_wdata,// 存储器写数据
    output [3:0]  mem_wmask // 存储器写掩码
);

    // -------------------------------
    // 指令解码 (RV32E)
    // -------------------------------
    wire [6:0] opcode = inst[6:0];
    wire [2:0] funct3 = inst[14:12];
    wire [6:0] funct7 = inst[31:25];
    wire [1:0] alu_op = alu_ctrl[1:0]; // 简化控制信号映射
    
    // -------------------------------
    // ALU计算逻辑 (RV32E) - 修复警告
    // -------------------------------
    reg [31:0] alu_result;
    
    always @(*) begin
        case (alu_op)
            // 算术运算（ADD/SUB）
            2'b00: begin
                case (funct3)
                    3'b000: alu_result = (funct7 == 7'b0100000) ? src1 - src2 : src1 + src2;
                    3'b001: alu_result = src1 << src2[4:0];
                    3'b010: alu_result = ($signed(src1) < $signed(src2)) ? 1 : 0;
                    3'b011: alu_result = (src1 < src2) ? 1 : 0;
                    3'b100: alu_result = src1 ^ src2;
                    3'b101: alu_result = (funct7 == 7'b0100000) ? $signed(src1) >>> src2[4:0] : src1 >> src2[4:0];
                    3'b110: alu_result = src1 | src2;
                    3'b111: alu_result = src1 & src2;
                    default: alu_result = 32'h0; // 添加默认分支
                endcase
            end
            
            // 逻辑运算（AND/OR/XOR）
            2'b01: begin
                case (funct3)
                    3'b000: alu_result = src1 & src2;
                    3'b001: alu_result = src1 | src2;
                    3'b010: alu_result = src1 ^ src2;
                    default: alu_result = 32'h0; // 添加默认分支
                endcase
            end
            
            // 移位运算（SLL/SRL/SRA）
            2'b10: begin
                case (funct3)
                    3'b000: alu_result = src1 << src2[4:0];
                    3'b001: alu_result = src1 >> src2[4:0];
                    3'b010: alu_result = $signed(src1) >>> src2[4:0];
                    default: alu_result = 32'h0; // 添加默认分支
                endcase
            end
            
            // 比较运算（SLT/SLTU）
            2'b11: begin
                case (funct3)
                    3'b000: alu_result = ($signed(src1) < $signed(src2)) ? 1 : 0;
                    3'b001: alu_result = (src1 < src2) ? 1 : 0;
                    default: alu_result = 32'h0; // 添加默认分支
                endcase
            end
            
            default: alu_result = 32'h0;
        endcase
    end
    
    assign result = alu_result;

    // -------------------------------
    // 分支跳转判断 (RV32E)
    // -------------------------------
    reg branch_taken;
    always @(*) begin
        branch_taken = 1'b0;
        if (opcode == 7'b1100011) begin // 分支指令
            case (funct3)
                3'b000: branch_taken = (src1 == src2); // BEQ
                3'b001: branch_taken = (src1 != src2); // BNE
                3'b100: branch_taken = ($signed(src1) < $signed(src2)); // BLT
                3'b101: branch_taken = ($signed(src1) >= $signed(src2)); // BGE
                3'b110: branch_taken = (src1 < src2); // BLTU
                3'b111: branch_taken = (src1 >= src2); // BGEU
                default: branch_taken = 1'b0;
            endcase
        end
    end

    // -------------------------------
    // 下一条PC计算 (RV32E)
    // -------------------------------
    assign next_pc = 
        (opcode == 7'b1101111) ? (pc + imm) :        // JAL
        (opcode == 7'b1100111) ? (src1 + imm) & ~1 : // JALR
        branch_taken ? (pc + imm) :                  // 分支跳转
        (pc + 4);                                    // 默认顺序执行

    // -------------------------------
    // 存储器控制 (RV32E)
    // -------------------------------
    assign mem_wren = (opcode == 7'b0100011); // Store指令
    assign mem_addr = src1 + imm;              // 地址计算
    assign mem_wdata = src2;                   // 写数据
    
    // 写掩码生成
    assign mem_wmask = 
        (funct3 == 3'b000) ? 4'b0001 : // SB (1字节)
        (funct3 == 3'b001) ? 4'b0011 : // SH (2字节)
        4'b1111;                       // SW (4字节)

    // -------------------------------
    // 调试输出
    // -------------------------------
    always @(posedge clk) begin
        if (rst) begin
            $display("[EXU] Reset");
        end else if (branch_taken) begin
            $display("[EXU] Branch taken at pc=0x%08x", pc);
        end
    end

endmodule