module ysyx_25040118_top (
    input         clk,      // 时钟输入
    input         rst_n     // 复位输入（低有效）
);

    // -------------------------------
    // 内部信号定义 (RV32E 优化)
    // -------------------------------
    wire rst = ~rst_n; // 转换为高有效复位
    wire [31:0] pc, next_pc, inst;
    wire [3:0]  rd, rs1, rs2;  // 4位寄存器索引 (0-15)
    wire [31:0] imm;
    wire [6:0]  opcode;
    wire        ebreak;
    wire [4:0]  alu_ctrl;
    wire [31:0] exu_result, reg_src1, reg_src2;
    wire        mem_wren;
    wire [31:0] mem_addr, mem_wdata;
    wire [3:0]  mem_wmask;
    wire [31:0] data_rdata;

    // -------------------------------
    // 模块实例化 (RV32E 优化)
    // -------------------------------
    // IFU（取指单元）
    ysyx_25040118_ifu ifu (
        .clk(clk),
        .rst(rst),
        .next_pc(next_pc),
        .pc(pc),
        .inst(inst)
    );

    // IDU（译码单元）
    ysyx_25040118_idu idu (
        .inst(inst),
        .pc(pc),
        .rd(rd),
        .rs1(rs1),
        .rs2(rs2),
        .imm(imm),
        .opcode(opcode),
        .ebreak(ebreak),
        .alu_ctrl(alu_ctrl)
    );

    // 寄存器文件 (RV32E 专用)
    ysyx_25040118_Registers reg_file (
        .clk(clk),
        .rst(rst),
        .waddr(rd),
        .wdata(exu_result),
        .wren((opcode != 7'b1100011 && opcode != 7'b0100011 && rd != 0)), // 非分支/跳转且非0号寄存器
        .raddr1(rs1),
        .rdata1(reg_src1),
        .raddr2(rs2),
        .rdata2(reg_src2)
    );

    // EXU（执行单元）
    ysyx_25040118_exu exu (
        .clk(clk),
        .rst(rst),
        .pc(pc),
        .inst(inst),
        .src1(reg_src1),
        .src2(reg_src2),
        .imm(imm),
        .alu_ctrl(alu_ctrl),
        .result(exu_result),
        .next_pc(next_pc),
        .mem_wren(mem_wren),
        .mem_addr(mem_addr),
        .mem_wdata(mem_wdata),
        .mem_wmask(mem_wmask)
    );

    // MEM（存储器单元）
    ysyx_25040118_mem mem (
        .clk(clk),
        .rst(rst),
        .inst_addr(pc),
        .inst_data(inst),
        .data_we(mem_wren),
        .data_addr(mem_addr),
        .data_wdata(mem_wdata),
        .data_wmask(mem_wmask),
        .data_rdata(data_rdata)
    );

    // -------------------------------
    // 顶层逻辑（EBREAK处理）
    // -------------------------------
    always @(posedge clk) begin
        if (ebreak) begin
            $display("[TOP] EBREAK encountered at pc=0x%08x", pc);
            $finish; // 触发仿真结束
        end
    end

endmodule
