`timescale 1ns/1ns
module stimulus_1ddct;

parameter clk_time = 5;
parameter max_size = 16; 

wire [11:0] out;


reg rstn, clk;
reg [1:0] state;
reg [7:0] in;


integer i, j;
integer handle1;

loeffler_1d dct_1d (clk, rstn, in, out, state);
 
reg [7:0] mem0 [0:63];
// reg [7:0] mem0 [0:63];
  
initial
begin
	clk=0;
	forever 
	#clk_time clk=~clk;	//Set clock with a period 10 units  5+5 10ns => 100Mhz
end
  
initial
begin

    //handle1 = $fopen("out.txt");
    // $readmemh("hex.txt", mem0);
    $readmemh("hex.txt", mem0);
    
    rstn = 1'b0;
    i = 0;
	state = 0;
    #10;
    rstn = 1'b1;
    
    for (i = 0; i <= 64; i = i + 1) begin	  
		if(i % 8 == 0)
		begin
			state = 1;
			#10;
			state = 2;
			#50;
			rstn = 0;
			state = 3;
			#100;
		end
		rstn = 1;
		state = 0;
		in = mem0[i];
		#10;
    end

	state = 0;
	// for (i = 0; i < 63; i = i + 8)
    // begin	  
	// 	win0 = mem0[i];
	// 	win1 = mem0[i+1];
	// 	win2 = mem0[i+2]; 
	// 	win3 = mem0[i+3]; 
	// 	win4 = mem0[i+4]; 
	// 	win5 = mem0[i+5]; 
	// 	win6 = mem0[i+6]; 
	// 	win7 = mem0[i+7]; 
	// 	#10;
    // end

	#10;

    //$fdisplay(handle1," %d, %d, %d, %d, %d, %d, %d, %d,", out0, out1, out2, out3, out4, out5, out6, out7);
	
	//#200 $fclose(handle1);
    #10 $finish; 
         
end

initial begin
	$dumpfile("1d_DCT.vcd");
    $dumpvars(0, stimulus_1ddct);
end

endmodule
