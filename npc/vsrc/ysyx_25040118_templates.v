module ysyx_25040118_Reg #(WIDTH = 1, RESET_VAL = 0) (
    input clk,
    input rst,
    input [WIDTH-1:0] din,
    output reg [WIDTH-1:0] dout,
    input wen
);
    always @(posedge clk) begin
        if (rst) begin
            dout <= RESET_VAL;
        end
        else if (wen) begin
            dout <= din;
        end
    end
endmodule

module ysyx_25040118_MuxKey #(NR_KEY = 2, KEY_LEN = 1, DATA_LEN = 1) (//无默认值选择器:未命中时输出全0
    output [DATA_LEN-1:0] out,
    input [KEY_LEN-1:0] key,
    input [NR_KEY*(KEY_LEN + DATA_LEN)-1:0] lut
);
    ysyx_25040118_MuxKeyInternal #(NR_KEY, KEY_LEN, DATA_LEN, 0) i0 (out, key, {DATA_LEN{1'b0}}, lut);
endmodule

module ysyx_25040118_MuxKeyWithDefault #(NR_KEY = 2, KEY_LEN = 1, DATA_LEN = 1) (//带默认值选择器:未命中时输出default_out
    output [DATA_LEN-1:0] out,
    input [KEY_LEN-1:0] key,
    input [DATA_LEN-1:0] default_out,
    input [NR_KEY*(KEY_LEN + DATA_LEN)-1:0] lut
);
    ysyx_25040118_MuxKeyInternal #(NR_KEY, KEY_LEN, DATA_LEN, 1) i0 (out, key, default_out, lut);
endmodule

module ysyx_25040118_MuxKeyInternal #(NR_KEY = 2, KEY_LEN = 1, DATA_LEN = 1, HAS_DEFAULT = 0) (//选择器内部实现:把LUT拆成key/data再匹配输出
    output reg [DATA_LEN-1:0] out,
    input [KEY_LEN-1:0] key,
    input [DATA_LEN-1:0] default_out,
    input [NR_KEY*(KEY_LEN + DATA_LEN)-1:0] lut
);
    localparam PAIR_LEN = KEY_LEN + DATA_LEN;//每组{key,data}的位宽
    wire [PAIR_LEN-1:0] pair_list [NR_KEY-1:0];
    wire [KEY_LEN-1:0] key_list [NR_KEY-1:0];
    wire [DATA_LEN-1:0] data_list [NR_KEY-1:0];

    genvar n;
    generate
        for (n = 0; n < NR_KEY; n = n + 1) begin
            assign pair_list[n] = lut[PAIR_LEN*(n+1)-1 : PAIR_LEN*n];
            assign data_list[n] = pair_list[n][DATA_LEN-1:0];
            assign key_list[n]  = pair_list[n][PAIR_LEN-1:DATA_LEN];
        end
    endgenerate

    reg [DATA_LEN-1 : 0] lut_out;
    reg hit;
    integer i;
    always @(*) begin
        lut_out = 0;
        hit = 0;
        for (i = 0; i < NR_KEY; i = i + 1) begin
            //key命中时,把对应data按位与后并入结果
            lut_out = lut_out | ({DATA_LEN{key == key_list[i]}} & data_list[i]);
            hit = hit | (key == key_list[i]);
        end
        //HAS_DEFAULT=0时直接输出匹配结果;否则未命中回落到default_out
        if (!HAS_DEFAULT) begin
            out = lut_out;
        end
        else begin
            out = (hit ? lut_out : default_out);
        end
    end
endmodule
