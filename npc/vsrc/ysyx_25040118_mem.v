module ysyx_25040118_mem (
    input         clk,      // 时钟
    input         rst,      // 复位（低有效）
    input  [31:0] inst_addr,// 指令地址
    output [31:0] inst_data,// 指令数据
    input         data_we,  // 数据写使能
    input  [31:0] data_addr,// 数据地址
    input  [31:0] data_wdata,// 数据写数据
    input  [3:0]  data_wmask,// 数据写掩码
    output [31:0] data_rdata// 数据读数据
);

    // 内存边界定义 (128MB)
    localparam PMEM_BASE = 32'h8000_0000;
    localparam PMEM_SIZE = 32'h0800_0000;
    localparam PMEM_END  = PMEM_BASE + PMEM_SIZE - 1;
    
    // 地址有效性检查
    wire inst_addr_valid = (inst_addr >= PMEM_BASE) && (inst_addr <= PMEM_END);
    wire data_addr_valid = (data_addr >= PMEM_BASE) && (data_addr <= PMEM_END);
    
    // 指令内存
    reg [31:0] imem [0:1023];
    assign inst_data = inst_addr_valid ? imem[inst_addr[11:2]] : 32'h0000_0013; // 无效地址返回nop
    
    // 数据内存
    reg [31:0] dmem [0:1023];
    assign data_rdata = data_addr_valid ? dmem[data_addr[11:2]] : 32'h0;
    
    // 数据写入
    always @(posedge clk) begin
        if (data_we && data_addr_valid) begin
            case (data_wmask)
                4'b0001: dmem[data_addr[11:2]][7:0]   <= data_wdata[7:0]; // SB
                4'b0011: dmem[data_addr[11:2]][15:0]  <= data_wdata[15:0]; // SH
                4'b1111: dmem[data_addr[11:2]]         <= data_wdata; // SW
                default: ; 
            endcase
        end
    end
    
    // 地址越界警告
    always @(posedge clk) begin
        if (!rst) begin
            if (!inst_addr_valid) begin
                $display("[MEM] WARNING: Inst access out of bounds: 0x%08x", inst_addr);
            end
            if (data_we && !data_addr_valid) begin
                $display("[MEM] WARNING: Data write out of bounds: 0x%08x", data_addr);
            end
            if (!data_addr_valid) begin
                $display("[MEM] WARNING: Data read out of bounds: 0x%08x", data_addr);
            end
        end
    end

endmodule
