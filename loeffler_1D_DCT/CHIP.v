module CHIP ( clk, rstn, in, out, state);

input clk, rstn; 
input [1:0] state;
input [7:0] in;                
output wire [11:0] out;  

wire i_clk, i_rstn;
wire [1:0] i_state;
wire [7:0] i_in;
wire [11:0] i_out;

  loeffler_1d CORE ( .clk(i_clk), .rstn(i_rstn), .in(i_in), .out(i_out), .state(i_state));

  XMD ipad_clk    ( .I(clk), .PU(n_Logic0_), .PD(n_Logic0_), .SMT(n_Logic0_), .O(i_clk) );
  XMD ipad_rst    ( .I(rstn), .PU(n_Logic0_), .PD(n_Logic0_), .SMT(n_Logic0_), .O(i_rstn) );
  XMD ipad_state0 ( .I(state[0]),  .PU(n_Logic0_), .PD(n_Logic0_), .SMT(n_Logic0_), .O(i_state[0]));
  XMD ipad_state1 ( .I(state[1]),  .PU(n_Logic0_), .PD(n_Logic0_), .SMT(n_Logic0_), .O(i_state[1]));

  XMD ipad_in0    ( .I(in[0]), .PU(n_Logic0_), .PD(n_Logic0_), .SMT(n_Logic0_), .O(i_in[0]) );
  XMD ipad_in1    ( .I(in[1]), .PU(n_Logic0_), .PD(n_Logic0_), .SMT(n_Logic0_), .O(i_in[1]) );
  XMD ipad_in2    ( .I(in[2]), .PU(n_Logic0_), .PD(n_Logic0_), .SMT(n_Logic0_), .O(i_in[2]) );
  XMD ipad_in3    ( .I(in[3]), .PU(n_Logic0_), .PD(n_Logic0_), .SMT(n_Logic0_), .O(i_in[3]) );
  XMD ipad_in4    ( .I(in[4]), .PU(n_Logic0_), .PD(n_Logic0_), .SMT(n_Logic0_), .O(i_in[4]) );
  XMD ipad_in5    ( .I(in[5]), .PU(n_Logic0_), .PD(n_Logic0_), .SMT(n_Logic0_), .O(i_in[5]) );
  XMD ipad_in6    ( .I(in[6]), .PU(n_Logic0_), .PD(n_Logic0_), .SMT(n_Logic0_), .O(i_in[6]) );
  XMD ipad_in7    ( .I(in[7]), .PU(n_Logic0_), .PD(n_Logic0_), .SMT(n_Logic0_), .O(i_in[7]) );
  
  YA2GSD opad_out0  ( .I(i_out[0]),  .E(n_Logic1_), .E2(n_Logic1_), .E4(n_Logic1_), .E8(n_Logic1_), .SR(n_Logic1_), .O(out[0]) );
  YA2GSD opad_out1  ( .I(i_out[1]),  .E(n_Logic1_), .E2(n_Logic1_), .E4(n_Logic1_), .E8(n_Logic1_), .SR(n_Logic1_), .O(out[1]) );
  YA2GSD opad_out2  ( .I(i_out[2]),  .E(n_Logic1_), .E2(n_Logic1_), .E4(n_Logic1_), .E8(n_Logic1_), .SR(n_Logic1_), .O(out[2]) );
  YA2GSD opad_out3  ( .I(i_out[3]),  .E(n_Logic1_), .E2(n_Logic1_), .E4(n_Logic1_), .E8(n_Logic1_), .SR(n_Logic1_), .O(out[3]) );
  YA2GSD opad_out4  ( .I(i_out[4]),  .E(n_Logic1_), .E2(n_Logic1_), .E4(n_Logic1_), .E8(n_Logic1_), .SR(n_Logic1_), .O(out[4]) );
  YA2GSD opad_out5  ( .I(i_out[5]),  .E(n_Logic1_), .E2(n_Logic1_), .E4(n_Logic1_), .E8(n_Logic1_), .SR(n_Logic1_), .O(out[5]) );
  YA2GSD opad_out6  ( .I(i_out[6]),  .E(n_Logic1_), .E2(n_Logic1_), .E4(n_Logic1_), .E8(n_Logic1_), .SR(n_Logic1_), .O(out[6]) );
  YA2GSD opad_out7  ( .I(i_out[7]),  .E(n_Logic1_), .E2(n_Logic1_), .E4(n_Logic1_), .E8(n_Logic1_), .SR(n_Logic1_), .O(out[7]) );
  YA2GSD opad_out8  ( .I(i_out[8]),  .E(n_Logic1_), .E2(n_Logic1_), .E4(n_Logic1_), .E8(n_Logic1_), .SR(n_Logic1_), .O(out[8]) );
  YA2GSD opad_out9  ( .I(i_out[9]),  .E(n_Logic1_), .E2(n_Logic1_), .E4(n_Logic1_), .E8(n_Logic1_), .SR(n_Logic1_), .O(out[9]) );
  YA2GSD opad_out10 ( .I(i_out[10]), .E(n_Logic1_), .E2(n_Logic1_), .E4(n_Logic1_), .E8(n_Logic1_), .SR(n_Logic1_), .O(out[10]) );
  YA2GSD opad_out11 ( .I(i_out[11]), .E(n_Logic1_), .E2(n_Logic1_), .E4(n_Logic1_), .E8(n_Logic1_), .SR(n_Logic1_), .O(out[11]) );
  
  TIE0 U1 ( .O(n_Logic0_) );
  TIE1 U2 ( .O(n_Logic1_) );

endmodule