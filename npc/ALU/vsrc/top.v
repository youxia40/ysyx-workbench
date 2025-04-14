module top (
	input clk,
	input [2:0] choose,
	input [3:0] a,
	input [3:0] b,
	output reg cin,					//是否进位
	output reg m, 				  //是否溢出(仅需加/减时判断)
	output reg [3:0] out
);
	always @(posedge clk)
	begin
		case(choose)
			3'b000:begin 
			{cin,out}={1'b0,a}+{1'b0,b}; 
			m=a[3]&b[3]&~out[3] | ~a[3]&~b[3]&out[3];
			end
			3'b001:begin 
			{cin,out}=$signed(a)-$signed(b); 
			m=a[3]&b[3]&~out[3] | ~a[3]&~b[3]&out[3];
			end
			3'b010:begin 
			out=~a; 
			cin=1'b0;
			m=1'b0;
			end
			3'b011:begin 
			{cin,out}={1'b0,a}&{1'b0,b}; 
			m=1'b0;
			end
			3'b100:begin 
			{cin,out}={1'b0,a}|{1'b0,b};
			m=1'b0;
			end
			3'b101:begin 
			{cin,out}={1'b0,a}^{1'b0,b}; 
			m=1'b0;
			end
			3'b110:begin 
				    	if (a[3] == b[3]) 
				    		begin
                    				if (a < b) begin 
                    				out = 4'b0001;
                    				cin=1'b0;
                    				m=1'b0;
                    				end
                        			else  begin
                        			out = 4'b0000;
                        			cin=1'b0;
                        			m=1'b0;
                        			end
                				end
               				 else 
               				 	begin				
                    				if ((a[3] == 1'b0) && (b[3] == 1'b1)) begin
                    				out= 4'b0000;
                    				cin=1'b0;
                    				m=1'b0;
                    				end
                    				if ((a[3] == 1'b1) && (b[3] == 1'b0)) begin
                    				out= 4'b0001;
                    				cin=1'b0;
                    				m=1'b0;
                    				end
                				end
            			end
			3'b111:begin
					if(a==b) begin
					out=4'b0001;
					cin=1'b0;
					m=1'b0;
					end
					else begin
					out=4'b0000;
					cin=1'b0;
					m=1'b0;
					end
				end
		endcase
	end
endmodule
