module top (
	input clk,
	input open,
	output [7:0] x,
	output [6:0] l1,
	output [6:0] l2
);
reg [15:0] count=16'b0;
reg [6:0] light1,light2;
reg [7:0] led=8'b1;
reg bi;
always @(posedge clk)
begin
    if(open)
        begin
            if (count == 0) 
            begin
            bi = led[4]^led[3]^led[2]^led[0];
            led = {bi,led[7:1]};
            end
            if(count<5000)				//每5000个单位时间变化一次
     	    begin
      	 	count <= count+1;
      	    end
      	    else
      	    begin
      		count <= 16'b0;
      	    end
        end
   	if(led==8'b0)				//全零则赋为1
        begin
            led=8'b1;
        end
    case(led[7:4])
    	4'b0000:light1<=7'b1000000;
    	4'b0001:light1<=7'b1111001;
    	4'b0010:light1<=7'b0100100;
    	4'b0011:light1<=7'b0110000;
    	4'b0100:light1<=7'b0011001;
    	4'b0101:light1<=7'b0010010;
    	4'b0110:light1<=7'b0000010;
    	4'b0111:light1<=7'b1111000;
    	4'b1000:light1<=7'b0000000;
    	4'b1001:light1<=7'b0010000;
    	4'b1010:light1<=7'b0001000;
    	4'b1011:light1<=7'b0000011;
    	4'b1100:light1<=7'b1000110;
    	4'b1101:light1<=7'b0100001;
    	4'b1110:light1<=7'b0000110;
    	4'b1111:light1<=7'b0001110;
    endcase
    case(led[3:0])
    	4'b0000:light2<=7'b1000000;
    	4'b0001:light2<=7'b1111001;
    	4'b0010:light2<=7'b0100100;
    	4'b0011:light2<=7'b0110000;
    	4'b0100:light2<=7'b0011001;
    	4'b0101:light2<=7'b0010010;
    	4'b0110:light2<=7'b0000010;
    	4'b0111:light2<=7'b1111000;
    	4'b1000:light2<=7'b0000000;
    	4'b1001:light2<=7'b0010000;
    	4'b1010:light2<=7'b0001000;
    	4'b1011:light2<=7'b0000011;
    	4'b1100:light2<=7'b1000110;
    	4'b1101:light2<=7'b0100001;
    	4'b1110:light2<=7'b0000110;
    	4'b1111:light2<=7'b0001110;
    endcase
    	
end
assign x=led;
assign l1=light1;
assign l2=light2;
endmodule
