module ysyx_25040118_csr (
    input clk,
    input rst,
    input stop,
    input [11:0] csr_raddr,
    input [11:0] csr_waddr,
    input [31:0] csr_wdata,
    input csr_we,
    input trap_we,//写CSR时触发陷阱寄存器更新
    input [31:0] trap_epc,
    input [31:0] trap_cause,
    output reg [31:0] csr_rdata,
    output [31:0] csr_mtvec,
    output [31:0] csr_mepc
);

/*
M极csr常用列表:
    名称             编号         功能
    mstatus         0x300        机器状态寄存器,包含全局中断使能位和异常处理状态
    mepc            0x341        机器异常程序计数器,记录引发异常的指令地址
    mcause          0x342        机器异常原因寄存器,记录异常类型和中断/异常标志
    mtvec           0x305        机器异常向量基地址寄存器,存放异常处理函数入口地址
    mcycle          0xB00        机器周期计数器,记录处理器运行的周期数
    mcycleh         0xB80        机器周期计数器高位,当mcycle溢出时增加该寄存器的值
    mvendorid       0xF11        机器供应商ID寄存器,标识处理器供应商
    marchid         0xF12        机器架构ID寄存器,标识处理器架构
*/

    reg [31:0] mstatus;
    reg [31:0] mepc;
    reg [31:0] mcause;
    reg [31:0] mtvec;
    reg [31:0] mcycle;
    reg [31:0] mcycleh;

    localparam [31:0] MVENDORID_CONST = 32'h79737978;//ysyx
    localparam [31:0] MARCHID_CONST = 32'h017e14f6;//学号

    assign csr_mtvec = mtvec;//输出mtvec给EXU用于计算异常处理入口地址
    assign csr_mepc = mepc;

    always @(*) begin
        case (csr_raddr)
            12'h300: csr_rdata = mstatus;
            12'h305: csr_rdata = mtvec;
            12'h341: csr_rdata = mepc;
            12'h342: csr_rdata = mcause;
            12'hB00: csr_rdata = mcycle;
            12'hB80: csr_rdata = mcycleh;
            12'hF11: csr_rdata = MVENDORID_CONST;
            12'hF12: csr_rdata = MARCHID_CONST;
            default: csr_rdata = 32'b0;
        endcase
    end

    always @(posedge clk) begin
        if (rst) begin
            mstatus <= 32'h1800;//和nemuinit.c文件里面的mstatus初始值保持一致，保证diff测试机制正常工作
            mepc <= 32'b0;
            mcause <= 32'b0;
            mtvec <= 32'b0;
            mcycle <= 32'b0;
            mcycleh <= 32'b0;
        end else begin
            mcycle <= mcycle + 1;
            if (mcycle == 32'hffff_ffff) begin
                mcycleh <= mcycleh + 1;
            end

            if (trap_we) begin
                mepc <= trap_epc;
                mcause <= trap_cause;
            end

            if (csr_we) begin
                case (csr_waddr)
                    12'h300: mstatus <= csr_wdata;
                    12'h305: mtvec <= csr_wdata;
                    12'h341: mepc <= csr_wdata;
                    12'h342: mcause <= csr_wdata;
                    12'hB00: mcycle <= csr_wdata;
                    12'hB80: mcycleh <= csr_wdata;
                    default: ;
                endcase
            end
        end
    end

endmodule
