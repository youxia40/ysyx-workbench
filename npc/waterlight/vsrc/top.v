module top (
  input clk,
  input rst,
  output reg [15:0] led
);
  reg [7:0] count;
  always @(posedge clk) begin
    if (rst) begin led <= 16'b1; count <= 0; end
    else begin
      if (count == 0) 
      led <= {led[14:0], led[15]};
      if(count<8'b11111111)
      begin
      count <= count+1;
      end
      else
      begin
      count <= 8'b0;
      end
    end
  end
endmodule
