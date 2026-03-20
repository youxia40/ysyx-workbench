module ysyx_25040118_top (
    input clk,
    input rst,
    output [31:0] pc_out,
    output [31:0] inst_out,
    output reg stop
);


    wire [31:0] ifu_pc;//取指级当前PC
    wire [31:0] ifu_inst;//取指级当前指令
    wire [31:0] exu_next_pc;//执行级给出的下一PC

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

    wire idu_is_csrrw;
    wire idu_is_csrrs;
    wire idu_is_csrrc;
    wire idu_is_csrrwi;
    wire idu_is_csrrsi;
    wire idu_is_csrrci;
    wire idu_is_mret;
    wire idu_is_ecall;

    wire idu_is_auipc;
    wire idu_is_lui;
    wire idu_is_alu_imm;

    wire [31:0] regfile_rdata1; //rs1读数据
    wire [31:0] regfile_rdata2; //rs2读数据
    wire [31:0] exu_result;     //EXU结果
    wire [31:0] wb_data;        //WBU写回数据
    wire wb_wen;                //WBU写回使能


    wire [11:0] exu_csr_addr;
    wire [31:0] exu_csr_wdata;
    wire exu_csr_we;//EXU输出的CSR写使能
    wire exu_trap_we;//EXU触发的异常/中断写使能
    wire [31:0] exu_trap_epc;
    wire [31:0] exu_trap_cause;
    wire [31:0] csr_rdata;
    wire [31:0] csr_mtvec;//异常入口的地址,由CSR输给EXU
    wire [31:0] csr_mepc;



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
        .is_csrrw  (idu_is_csrrw),
        .is_csrrs  (idu_is_csrrs),
        .is_csrrc  (idu_is_csrrc),
        .is_csrrwi (idu_is_csrrwi),
        .is_csrrsi (idu_is_csrrsi),
        .is_csrrci (idu_is_csrrci),
        .is_mret   (idu_is_mret),
        .is_ecall  (idu_is_ecall),
        .is_auipc  (idu_is_auipc),
        .is_lui    (idu_is_lui),
        .is_alu_imm(idu_is_alu_imm)
    );


    ysyx_25040118_regfile regfile_module (//寄存器堆模块
        .clk   (clk),
        .rst   (rst),
        .stop  (stop),
        .waddr (idu_rd),
        .wdata (wb_data),
        .wen   (wb_wen),
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
        .is_csrrw (idu_is_csrrw),
        .is_csrrs (idu_is_csrrs),
        .is_csrrc (idu_is_csrrc),
        .is_csrrwi(idu_is_csrrwi),
        .is_csrrsi(idu_is_csrrsi),
        .is_csrrci(idu_is_csrrci),
        .is_mret  (idu_is_mret),
        .is_ecall (idu_is_ecall),
        .is_auipc (idu_is_auipc),
        .is_lui   (idu_is_lui),
        .is_alu_imm(idu_is_alu_imm),
        .csr_rdata(csr_rdata),
        .csr_mtvec(csr_mtvec),
        .csr_mepc (csr_mepc),
        .result   (exu_result),
        .next_pc  (exu_next_pc),
        .ebreak   (idu_ebreak),
        .csr_addr (exu_csr_addr),
        .csr_wdata(exu_csr_wdata),
        .csr_we   (exu_csr_we),
        .trap_we  (exu_trap_we),
        .trap_epc (exu_trap_epc),
        .trap_cause(exu_trap_cause)
    );

    ysyx_25040118_csr csr_module (
        .clk      (clk),
        .rst      (rst),
        .stop     (stop),
        .csr_raddr(exu_csr_addr),
        .csr_waddr(exu_csr_addr),
        .csr_wdata(exu_csr_wdata),
        .csr_we   (exu_csr_we),
        .trap_we  (exu_trap_we),
        .trap_epc (exu_trap_epc),
        .trap_cause(exu_trap_cause),
        .csr_rdata(csr_rdata),
        .csr_mtvec(csr_mtvec),
        .csr_mepc (csr_mepc)
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

    ysyx_25040118_wbu wbu_module (
        .rd         (idu_rd),
        .is_load    (idu_is_load),
        .is_store   (idu_is_store),
        .is_branch  (idu_is_branch),
        .is_system  (idu_is_system),
        .is_csrrw   (idu_is_csrrw),
        .is_csrrs   (idu_is_csrrs),
        .is_csrrc   (idu_is_csrrc),
        .is_csrrwi  (idu_is_csrrwi),
        .is_csrrsi  (idu_is_csrrsi),
        .is_csrrci  (idu_is_csrrci),
        .exu_result (exu_result),
        .lsu_data   (lsu_load_data),
        .wb_data    (wb_data),
        .wb_wen     (wb_wen)
    );



    //调试端口
    assign pc_out = ifu_pc;
    assign inst_out = ifu_inst;

    //死循环检测:如果PC长时间不变则置stop
    reg [31:0] last_pc;
    reg [31:0] same_pc_count;

    always @(posedge clk) begin
        if (rst) begin //复位时清空死循环检测状态
            last_pc <= 32'b0;
            same_pc_count <= 32'b0;
            stop <= 1'b0;
        end
        else if (!stop) begin //仅在运行态更新停机判定
            if (ifu_pc == last_pc) begin
                same_pc_count <= same_pc_count + 1;
                if (same_pc_count > 1000000) begin
                    stop <= 1'b1; //PC长期不变化则判定为死循环

                end
            end
            else begin
                last_pc <= ifu_pc;
                same_pc_count <= 32'b0;
            end
        end
    end
    
endmodule
