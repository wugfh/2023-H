`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2023/07/31 17:02:21
// Design Name: 
// Module Name: IIR
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////
`define DEF_FP_MUL(x,y,i0,f0,i1,f1,fr)\
    (((i0)+(i1)+(f0)+(f1))'(x)*((i0)+(i1)+(f0)+(f1))'(y))>>>((f0)+(f1)-(fr))
module IIR2nd#(
    parameter DW = 20,FW = 15
)(
    input clk,rst_n,en,
    input signed [DW-1:0] din,
    output logic signed[DW-1:0] dout,
    input [31:0]index
);
    logic signed[DW-1:0] GAIN =41;
    logic signed[DW-1:0] DEN[20]='{
        -65446,
        -65421,
        -65381,
        -65325,
        -65252,
        -65163,
        -65058,
        -64938,
        -64801,
        -64648,
        -64479,
        -64294,
        -64094,
        -63877,
        -63645,
        -63397,
        -63134,
        -62855,
        -62560,
        -62250
    };
    logic signed [DW-1:0] NUM[3] = '{32768,0,-32768};

    wire signed [DW-1:0] n0;
    wire signed [DW-1:0] n1;
    wire signed [DW-1:0] n2;
    wire signed [DW-1:0] d1;
    wire signed [DW-1:0] d2;
    wire signed [DW-1:0] g ;

    assign    n0      = (NUM[0]);
    assign    n1     = (NUM[1]);
    assign    n2     = (NUM[2]);
    assign    d1     = (DEN[index]);
    assign    d2     = (32686);
    assign    g      = (GAIN);

    `define mul(x,y) `DEF_FP_MUL(x,y,DW-FW,FW,DW-FW,FW,FW);
    logic signed [DW-1:0]   z1,z0;
    wire signed [DW-1:0] pn0;
    wire signed [DW-1:0] pn1;
    wire signed [DW-1:0] pn2;
    wire signed [DW-1:0] o ;
    wire signed [DW-1:0] pd1;
    wire signed [DW-1:0] pd2;

    assign        pn0    = `mul(din,n0);
    assign        pn1    = `mul(din,n1);
    assign        pn2    = `mul(din,n2);
    assign        o     =z0 + pn0;
    assign        pd1    = `mul(o,d1);
    assign        pd2    = `mul(o,d2);

    always_ff @( posedge clk ) begin 
        if(!rst_n) begin
            z0<=0; z1<=0;dout<=0;
        end else begin
            z0<=pn1 + z1 - pd1;
            z1<=pn2 - pd2;
            dout<=`mul(o,g);
        end
    end
endmodule

module IIR#(
    parameter DW = 16,EW = 4
)(
    input clk,rst_n,en,
    input signed [DW-1:0] din,
    output logic signed [DW-1:0] dout,
    input [31:0]index,
    output logic signed [DW+EW-1:0] dout_W
    );
    localparam W = DW + EW;
    logic signed [W-1:0] din_W;
    
    assign din_W = din;
    assign dout = dout_W;
    
    IIR2nd #(W,DW-1)IIR2nd_inst(
    clk,rst_n,en, din_W, dout_W,index
        );
endmodule