`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2023/08/02 21:09:16
// Design Name: 
// Module Name: IIR_v
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


module IIR_v#(
    parameter DW = 16,FW = 16
)(
    input clk,rst_n,en,
    input signed [DW-1:0] din,
    output wire signed [DW-1:0] dout,
    input [31:0]index,
    output wire signed [DW+FW-1:0] dout_W
    );

    IIR#(
        DW,FW
    )IIR_inst(
        clk,rst_n,en,din,dout,index,dout_W
    );
endmodule
