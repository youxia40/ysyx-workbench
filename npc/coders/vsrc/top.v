module top (
	input [7:0] x,
	input en,
	output reg k,
	output reg [2:0] y,
	output reg [6:0] z
);
	always @(*)
	  begin
	    if(en)
	      begin
		casez(x)
			8'b00000000:begin k=1'b0;y=3'b000; end
			8'b00000001:begin k=1'b1;y=3'b001; end
			8'b0000001?:begin k=1'b1;y=3'b010; end
			8'b000001??:begin k=1'b1;y=3'b011; end
			8'b00001???:begin k=1'b1;y=3'b100; end
			8'b0001????:begin k=1'b1;y=3'b101; end
			8'b001?????:begin k=1'b1;y=3'b110; end
			8'b01??????:begin k=1'b1;y=3'b111; end
			8'b1???????:begin k=1'b1;y=3'b000; end
		endcase
		case(y)
			3'b000:z=7'b1000000;
			3'b001:z=7'b1111001; 
			3'b010:z=7'b0100100;
			3'b011:z=7'b0110000;
			3'b100:z=7'b0011001;
			3'b101:z=7'b0010010;
			3'b110:z=7'b0000010;
			3'b111:z=7'b1111000;
		endcase
	      end
	    end
endmodule
