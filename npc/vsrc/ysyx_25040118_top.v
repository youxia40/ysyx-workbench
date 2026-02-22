module ysyx_25040118_top (//顶层模块:连接IFU/IDU/EXU/LSU/RegFile并导出调试端口
    input clk,
    input rst,
    output [31:0] pc_out,
    output [31:0] inst_out,
    output reg stop
);


    wire [31:0] ifu_pc;      //取指级当前PC
    wire [31:0] ifu_inst;    //取指级当前指令
    wire [31:0] exu_next_pc; //执行级给出的下一PC

    wire [4:0] idu_rd;
    wire [4:0] idu_rs1;
    wire [4:0] idu_rs2;
    wire [31:0] idu_imm;
    wire [4:0] idu_alu_ctrl;
    wire idu_ebreak;
    wire idu_is_load;
    wire idu_is_store;
    wire idu_is_branch;
    wire idu_is_jal;
    wire idu_is_jalr;
    wire idu_is_system;
    wire idu_is_auipc;
    wire idu_is_lui;
    wire idu_is_alu_imm;

    wire [31:0] regfile_rdata1; //rs1读数据
    wire [31:0] regfile_rdata2; //rs2读数据
    wire [31:0] exu_result;     //写回数据


    //寄存器写使能:rd非x0且不是store/branch/system指令
    wire reg_wen = (|idu_rd) && !idu_is_store && !idu_is_branch && !idu_is_system;



    ysyx_25040118_ifu ifu_module (//取指模块
        .clk    (clk),
        .rst    (rst),
        .stop   (stop),
        .next_pc(exu_next_pc),
        .pc     (ifu_pc),
        .inst   (ifu_inst)
    );


    ysyx_25040118_idu idu_module (//译码模块
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


    ysyx_25040118_regfile regfile_module (//寄存器堆模块
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


    ysyx_25040118_exu exu_module (//执行模块
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
        .lsu_load_data(lsu_load_data),
        .next_pc  (exu_next_pc),
        .ebreak   (idu_ebreak)
    );

    wire [31:0] lsu_load_data; //访存返回的load数据
    ysyx_25040118_lsu lsu_module (
        .clk      (clk),
        .rst      (rst),
        .stop     (stop),
        .inst     (ifu_inst),
        .src1     (regfile_rdata1),
        .src2     (regfile_rdata2),
        .imm      (idu_imm),
        .is_load  (idu_is_load),
        .is_store (idu_is_store),
        .load_data(lsu_load_data)
    );



    //导出调试端口
    assign pc_out   = ifu_pc;
    assign inst_out = ifu_inst;

    //死循环检测:如果PC长时间不变则置stop
    reg [31:0] last_pc;
    reg [31:0] same_pc_count;

    always @(posedge clk) begin
        if (rst) begin //复位时清空死循环检测状态
            last_pc       <= 32'b0;
            same_pc_count <= 32'b0;
            stop          <= 1'b0;
        end
        else if (!stop) begin //仅在运行态更新停机判定
            if (ifu_pc == last_pc) begin
                same_pc_count <= same_pc_count + 1;
                if (same_pc_count > 100) begin
                    stop <= 1'b1; //PC长期不变化则判定为死循环

                end
            end
            else begin
                last_pc       <= ifu_pc;
                same_pc_count <= 32'b0;
            end
        end
    end
endmodule
