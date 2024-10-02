
`timescale 1ns/1ns

module alu_tb;
reg clk, en, rstn;
reg [2:0] sel;
reg [7:0] x, y;                
wire [15:0] z;     

integer i, j;

alu uut(.clk(clk), .rstn(rstn), .en(en), .sel(sel), .x(x), .y(y), .z(z));

//module alu (z , clk , rstn , en , y , x , sel );

initial
begin
	clk = 1'b0;
	forever #5 clk = ~clk;
end
	  
initial 
begin
	en=0;
	rstn=0;
	sel=0;
	x=0;
	y=0;
	#10;	

	//endable
	en=1;
	rstn=1;
	
	//add testbench
	sel=0;
	x=5;
	y=5;
	#10;

	//sub testbench
	sel=1;
	x=10;
	y=3;
	#10;
	
	//mul testbench
	sel=2;
	x=5;
	y=5;
	#10;

	//shit left testbench
	sel=3;
	x=64;
	y=2;
	#10;

	//shit right testbench
	sel=4;
	x=64;
	y=2;
	#10;

	//mul1 test testbench
	sel=5;
	x=2;
	y=3;
	#10;

	//mul1 test testbench
	sel=6;
	x=2;
	y=3;
	#10;


	//mul1 test testbench
	sel=7;
	x=2;
	y=3;
	#10;


	$stop;
end

endmodule

