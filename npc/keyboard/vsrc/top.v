module top (
	input clk,ps2_data,ps2_clk,		//ps_data就是接受到的数据
	input clrn,
    	input nextdata_n,		//读取下一个数据
    	output reg ready,		//数据就绪
    	output reg [7:0] ascii_code,	//ascii码
    	output reg [13:0] ascii_code_light,
    	output reg [7:0] scan_code,	//扫描码
    	output reg [13:0] scan_code_light,
    	output reg [7:0] keystroke,	//按键次数
    	output reg [13:0] keystroke_light,
    	output reg [13:0] light_black
);
    reg [9:0] buffer;  		//存储10位扫描码数据
    reg [7:0] fifo[7:0];	//暂存
    reg [2:0] w_ptr,r_ptr; 	//读写指针
    reg [3:0] count; 		//计算数据位
    reg [2:0] ps2_clk_sync;	//ps2_clk时钟统计
    reg break_received;      // 断码接收标志
    reg [7:0] current_key;   // 当前按下的键
    
    assign light_black[13:0] = 14'b11111111111111;
    
    //ps2_data停止位为高电平1
    wire valid = (buffer[0]==0) & ps2_data & (^buffer[9:1]);     //valid用于判断数据是否有效
    
    always @(posedge clk) begin
        ps2_clk_sync <=  {ps2_clk_sync[1:0],ps2_clk};
    end
    wire sampling = ps2_clk_sync[2] & ~ps2_clk_sync [1];    //ps2_clk下降沿时sampling为1,结合上一个always一起看
     
     reg [7:0] scan[255:0];	//扫描码转ascii码
     always @(posedge clk) begin
         if (clrn == 0) begin
     
     scan[8'h1C]=8'h41;scan[8'h32]=8'h42;scan[8'h21]=8'h43;scan[8'h23]=8'h44;scan[8'h24]=8'h45;
     scan[8'h2B]=8'h46;scan[8'h34]=8'h47;scan[8'h33]=8'h48;scan[8'h43]=8'h49;scan[8'h3B]=8'h4A;
     scan[8'h42]=8'h4B;scan[8'h4B]=8'h4C;scan[8'h3A]=8'h4D;scan[8'h31]=8'h4E;scan[8'h44]=8'h4F;
     scan[8'h4D]=8'h50;scan[8'h15]=8'h51;scan[8'h2D]=8'h52;scan[8'h1B]=8'h53;scan[8'h2C]=8'h54;
     scan[8'h3C]=8'h55;scan[8'h2A]=8'h56;scan[8'h1D]=8'h57;scan[8'h22]=8'h58;scan[8'h35]=8'h59;
     scan[8'h1A]=8'h5A;scan[8'h76]=8'h1B;scan[8'h05]=8'h70;scan[8'h06]=8'h71;scan[8'h04]=8'h72;
     scan[8'h0C]=8'h73;scan[8'h03]=8'h74;scan[8'h0B]=8'h75;scan[8'h83]=8'h76;scan[8'h0A]=8'h77;
     scan[8'h01]=8'h78;scan[8'h09]=8'h79;scan[8'h78]=8'h7A;scan[8'h07]=8'h7B;scan[8'h0E]=8'h60;
     scan[8'h16]=8'h31;scan[8'h1E]=8'h32;scan[8'h26]=8'h33;scan[8'h25]=8'h34;scan[8'h2E]=8'h35;
     scan[8'h36]=8'h36;scan[8'h3D]=8'h37;scan[8'h3E]=8'h38;scan[8'h46]=8'h39;scan[8'h45]=8'h30;
     scan[8'h4E]=8'h2D;scan[8'h55]=8'h3D;scan[8'h5D]=8'h7C;scan[8'h66]=8'h7F;scan[8'h0D]=8'h09;
     scan[8'h58]=8'h14;scan[8'h12]=8'h10;scan[8'h14]=8'h11;scan[8'h11]=8'h12;scan[8'h29]=8'h20;
     scan[8'h54]=8'h5B;scan[8'h5B]=8'h5D;scan[8'h4C]=8'h3B;scan[8'h52]=8'h27;scan[8'h5A]=8'h0D;
     scan[8'h41]=8'h2C;scan[8'h49]=8'h2E;scan[8'h4A]=8'h2F;scan[8'h59]=8'h10;
         end
     end
     
     always @(posedge clk) begin
        if (clrn) begin			 // =1复位
            count <= 0; w_ptr <= 0; r_ptr <= 0;ready<= 0; keystroke<=0;
            ascii_code <= 0;;scan_code <= 0;;break_received <= 0; current_key <= 0;
            
        end
        //count=10,读入10位数据，本身就可以代表到了停止位
        else begin
         if (sampling & (count==10) & valid) begin
                scan_code <= buffer[8:1];
                ascii_code <= scan[buffer[8:1]];	  // 直接查表转换
            end
            
            //按键次数
            if (sampling & (count==10) & valid) begin
            
                if (buffer[8:1] == 8'hF0) begin
                    break_received <= 1;
                end else if (break_received) begin
                    break_received <= 0;
                    current_key <= 0;				//按键清零
                    //直接清零输出信号
                    scan_code <= 8'h00; 
                    ascii_code <= 8'h00;
                end else if (current_key != buffer[8:1]) begin
                    current_key <= buffer[8:1];
                    keystroke <= keystroke + 1;
                end
            end
            
            if (sampling) begin
              if (count == 4'd10) begin   //接收了10b数据
                if ((buffer[0] == 0) &&  // start bit=0
                    (ps2_data)       &&  // stop bit=1
                    (^buffer[9:1])) begin      // 奇偶校验
                    fifo[w_ptr] <= buffer[8:1];  // 存储扫描码
                    w_ptr <= w_ptr+1;
                      
                end
                count <= 0;     // for next重置
              end else begin
                
                buffer[count] <= ps2_data;  //ps2_data数据存储
                count <= count + 1;
              end
            end
            
            ready <= (w_ptr != r_ptr);      //判断是否有数据可读

            if (ready & ~nextdata_n) begin  //读取数据（nextdata设为计数归零信号，为1则清）
                r_ptr <= r_ptr + 1;
            end
        end
    end

    
    	light a1(.clk(clk),.led(ascii_code),.y(ascii_code_light));
    	light s1(.clk(clk),.led(scan_code),.y(scan_code_light));
    	light k1(.clk(clk),.led(keystroke),.y(keystroke_light));
endmodule

module light(
	input clk,
	input [7:0] led,
	output [13:0] y
);
reg [6:0] light1,light2;
    always @(posedge clk) begin
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
    assign y[13:0] = {light1[6:0],light2[6:0]};
endmodule
