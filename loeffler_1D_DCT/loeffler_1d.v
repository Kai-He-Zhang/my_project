module loeffler_1d(clk, rstn, in, out, state);

parameter max_size = 12;

input clk, rstn;
input [1:0] state;
input [7:0] in;
output reg [max_size-1:0] out;

reg [3:0] counter1, counter4;
reg signed [max_size-1:0] o0, o1, o2, o4, o6, o7;
reg signed [21:0] o3, o5;
reg signed [max_size-1:0] s10, s11, s12, s13, s14, s15, s16, s17;
reg signed [max_size-1:0] s20, s21, s22, s23;
reg signed [21:0] s24, s25, s26, s27, s32, s33;
reg signed [max_size-1:0] s30, s31, s34, s35, s36, s37;
wire signed [21:0] tmp3,  tmp6,  tmp9;
wire signed [21:0] tmp4, tmp5, tmp7, tmp8, tmp1, tmp2;
reg [7:0] in_mem [0:7];
reg [11:0] out_mem [0:7];

//b sin //a cos   //中間的a和m代表加和減
//b-a sin - cos   //sq表示有根號2
//b+a sin + cos   //數字表示對應的kcn n數值

parameter c1as1 = 1204;   
parameter s1mc1 = -804;    
parameter c1 = 1004;     
parameter c3as3 = 1420;  
parameter s3mc3 = -282;     
parameter c3 = 851;     
parameter sqc1as1 = 1892;
parameter sqs1mc1 = 784;
parameter sqc1 = 554;   
parameter sq2 = 1448; 


always @(posedge clk) begin
    if(rstn == 0) begin
        counter1 <= 0;       
    end else if(state == 0)begin
        in_mem[counter1] <= in;
        counter1 <= counter1 + 1;
    end else begin
        in_mem[counter1] <= in_mem[counter1];
        counter1 <= counter1;
    end
end


//stage = 1/////////////////////////////////////////////////////////////////////

always @(posedge clk) begin
    if(state == 1) begin
        s10 <= in_mem[0] + in_mem[7];
        s17 <= in_mem[0] - in_mem[7];
        s11 <= in_mem[1] + in_mem[6];
        s16 <= in_mem[1] - in_mem[6];
        s12 <= in_mem[2] + in_mem[5];
        s15 <= in_mem[2] - in_mem[5];
        s13 <= in_mem[3] + in_mem[4];
        s14 <= in_mem[3] - in_mem[4];
    end
end

//stage = 2/////////////////////////////////////////////////////////////////////


assign tmp4 = (s17 * s3mc3);
assign tmp5 = (s14 * c3as3);
assign tmp7 = (s16 * s1mc1);
assign tmp8 = (s15 * c1as1);
assign tmp9 = (s15 + s16) * c1;
assign tmp6 = (s17 + s14) * c3;

always @(posedge clk) begin
    s20 <= s10 + s13;
    s23 <= s10 - s13;
    s21 <= s11 + s12;
    s22 <= s11 - s12;
    s24 <= (tmp4 + tmp6) >> 10;
    s27 <= (tmp6 - tmp5) >> 10;
    s25 <= (tmp7 + tmp9) >> 10;
    s26 <= (tmp9 - tmp8) >> 10;
end

//stage = 3/////////////////////////////////////////////////////////////////////

assign tmp1 = (s23 * sqs1mc1);
assign tmp2 = (s22 * sqc1as1);
assign tmp3 = (s22 + s23) * sqc1;

always @(posedge clk) begin
    s30 <= s20 + s21;
    s31 <= s20 - s21;
    s32 <= (tmp1 + tmp3) >> 10;
    s33 <= (tmp3 - tmp2) >> 10;
    s34 <= s24 + s26;
    s36 <= s24 - s26;
    s37 <= s27 + s25;
    s35 <= s27 - s25;
end

//stage = 4/////////////////////////////////////////////////////////////////////

always @(posedge clk) begin
    o0 <= s30;
    o4 <= s31;
    o2 <= s32;
    o6 <= s33;
    o1 <= s37 + s34;
    o7 <= s37 - s34;
    o3 <= (s35 * sq2) >> 10;
    o5 <= (s36 * sq2) >> 10;
end

always @(posedge clk) begin
    if(state == 2) begin
        out_mem[0] <= o0;
        out_mem[1] <= o1;
        out_mem[2] <= o2;
        out_mem[3] <= o3;
        out_mem[4] <= o4;
        out_mem[5] <= o5;
        out_mem[6] <= o6;
        out_mem[7] <= o7;
    end
end

always @(posedge clk) begin
    if(state == 3) begin
        out <= out_mem[counter4];
        counter4 <= counter4 + 1;
    end else begin
        counter4 <= 0;
    end
end


endmodule