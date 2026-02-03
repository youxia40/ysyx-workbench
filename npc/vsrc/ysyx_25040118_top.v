module ysyx_25040118_top (
    input         clk,
    input         rst,
    output [31:0] pc_out,
    output [31:0] inst_out,
    output reg    stop
);


    wire [31:0] ifu_pc;
    wire [31:0] ifu_inst;
    wire [31:0] exu_next_pc;

    wire [4:0]  idu_rd;
    wire [4:0]  idu_rs1;
    wire [4:0]  idu_rs2;
    wire [31:0] idu_imm;
    wire [4:0]  idu_alu_ctrl;
    wire        idu_ebreak;
    wire        idu_is_load;
    wire        idu_is_store;
    wire        idu_is_branch;
    wire        idu_is_jal;
    wire        idu_is_jalr;
    wire        idu_is_system;
    wire        idu_is_auipc;
    wire        idu_is_lui;
    wire        idu_is_alu_imm;

    wire [31:0] regfile_rdata1;
    wire [31:0] regfile_rdata2;
    wire [31:0] exu_result;


    //寄存器写使能
    wire reg_wen = (|idu_rd) && !idu_is_store && !idu_is_branch && !idu_is_system;



    ysyx_25040118_ifu ifu_module (
        .clk    (clk),
        .rst    (rst),
        .stop   (stop),
        .next_pc(exu_next_pc),
        .pc     (ifu_pc),
        .inst   (ifu_inst)
    );


    ysyx_25040118_idu idu_module (
        .clk       (clk),
        .rst       (rst),
        .stop      (stop),
        .inst      (ifu_inst),
        .pc        (ifu_pc),
        .rd        (idu_rd),
        .rs1       (idu_rs1),
        .rs2       (idu_rs2),
        .imm       (idu_imm),
        .alu_ctrl  (idu_alu_ctrl),
        .ebreak    (idu_ebreak),
        .is_load   (idu_is_load),
        .is_store  (idu_is_store),
        .is_branch (idu_is_branch),
        .is_jal    (idu_is_jal),
        .is_jalr   (idu_is_jalr),
        .is_system (idu_is_system),
        .is_auipc  (idu_is_auipc),
        .is_lui    (idu_is_lui),
        .is_alu_imm(idu_is_alu_imm)
    );


    ysyx_25040118_regfile regfile_module (
        .clk   (clk),
        .rst   (rst),
        .stop  (stop),
        .waddr (idu_rd),
        .wdata (exu_result),
        .wen   (reg_wen),
        .raddr1(idu_rs1),
        .rdata1(regfile_rdata1),
        .raddr2(idu_rs2),
        .rdata2(regfile_rdata2)
    );


    ysyx_25040118_exu exu_module (
        .clk      (clk),
        .rst      (rst),
        .stop     (stop),
        .pc       (ifu_pc),
        .inst     (ifu_inst),
        .src1     (regfile_rdata1),
        .src2     (regfile_rdata2),
        .imm      (idu_imm),
        .alu_ctrl (idu_alu_ctrl),
        .is_load  (idu_is_load),
        .is_store (idu_is_store),
        .is_branch(idu_is_branch),
        .is_jal   (idu_is_jal),
        .is_jalr  (idu_is_jalr),
        .is_system(idu_is_system),
        .is_auipc (idu_is_auipc),
        .is_lui   (idu_is_lui),
        .is_alu_imm(idu_is_alu_imm),
        .result   (exu_result),
        .next_pc  (exu_next_pc),
        .ebreak   (idu_ebreak)
    );

    //输出端口
    assign pc_out   = ifu_pc;
    assign inst_out = ifu_inst;

    //死循环检测（PC如果长时间不变）
    reg [31:0] last_pc;
    reg [31:0] same_pc_count;

    always @(posedge clk) begin
        if (rst) begin
            last_pc       <= 32'b0;
            same_pc_count <= 32'b0;
            stop          <= 1'b0;
        end
        else if (!stop) begin
            if (ifu_pc == last_pc) begin
                same_pc_count <= same_pc_count + 1;
                if (same_pc_count > 100) begin
                    stop <= 1'b1;
                    //$strobe("[TOP] Deadloop detected at PC=0x%08x", ifu_pc);
                end
            end
            else begin
                last_pc       <= ifu_pc;
                same_pc_count <= 32'b0;
            end
        end
    end
endmodule
