module ysyx_25040118_regfile (//寄存器堆:维护RV32E通用寄存器读写
    input clk,
    input rst,
    input stop,
    input [4:0] waddr,
    input [31:0] wdata,
    input wen,
    input [4:0] raddr1,
    output [31:0] rdata1,
    input [4:0] raddr2,
    output [31:0] rdata2
);

    //每次写寄存器时,把新值同步到npc_ctx.debug.regs
    `ifndef SYNTHESIS
    import "DPI-C" function void npc_set_reg(input int idx, input int value);
    `endif


    logic [31:0] rf [0:15];

    //写端口:复位清零;正常时在wen有效且waddr!=0时写入
    always @(posedge clk) begin
        if (rst) begin
            for (integer i = 0; i < 16; i = i + 1) begin
                rf[i] <= 32'b0;
            end
        end
        else if (wen && waddr != 0) begin
            if (waddr < 16) begin
                rf[waddr] <= wdata;


                `ifndef SYNTHESIS
                //同步到调试上下文(供sdb/expr/difftest与trap打印使用)
                npc_set_reg(waddr, wdata);
                `endif


                //预留调试输出位置:仅在非stop且写入值非0时触发
                if (!stop && wdata != 0) begin


                end
            end
        end
    end

    //读端口:x0恒为0;RV32E仅支持x0~x15,越界读返回0
    assign rdata1 = (raddr1 == 0) ? 32'b0 :
                    (raddr1 < 16) ? rf[raddr1] : 32'b0;

    assign rdata2 = (raddr2 == 0) ? 32'b0 :
                    (raddr2 < 16) ? rf[raddr2] : 32'b0;

endmodule
