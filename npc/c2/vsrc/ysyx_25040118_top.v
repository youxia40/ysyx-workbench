module ysyx_25040118_top(
    input clk,
    input rst,
    output [31:0] pc/* verilator public */,
    output [4:0]  reg_waddr/* verilator public */,   // 目标寄存器地址
    output [31:0] reg_wdata/* verilator public */,     // 写入数据

    output [31:0] ins /* verilator public */,                       // 直接传递IDU的输入指令
    output [4:0] rd_addr /* verilator public */,                    // 目标寄存器地址
    output wen /* verilator public */
);

    wire [4:0]  rs1_addr/* verilator public *//* verilator public */;
    wire [31:0] imm;
    wire [31:0] rs1_data/* verilator public */;
    wire [31:0] alu_result/* verilator public */;
    wire [31:0] next_pc = pc + 4;  // PC自增逻辑

    wire ebreak_on;

    assign reg_waddr = rd_addr;
    assign reg_wdata = alu_result;


    // 模块实例化
    ysyx_25040118_ifu ifu_inst (
        .clk(clk),
        .reset(rst),
        .next_pc(next_pc),
        .pc(pc)
    );

    ysyx_25040118_mem mem_inst (
        .addr(pc),
        .ins(ins)
    );

    ysyx_25040118_idu idu_inst (
        .ins(ins),
        .wen(wen),
        .rs1_addr(rs1_addr),
        .rd_addr(rd_addr),
        .imm(imm),
        .ebreak_on(ebreak_on)
    );

    ysyx_25040118_Registers reg_inst (
        .clk(clk),
        .wdata(alu_result),
        .waddr(rd_addr),
        .wen(wen && (rd_addr != 0)),  // x0寄存器不可写
        .rdata1(rs1_data),
        .raddr1(rs1_addr)
    );

    ysyx_25040118_exu exu_inst (
        .rs1(rs1_data),
        .imm(imm),
        .outdata(alu_result)
    );


    //ebreak触发
    import "DPI-C" function void ebreak_trigger();
    always @(negedge clk) begin
        if (ebreak_on) begin
            ebreak_trigger();
        end
    end
endmodule
