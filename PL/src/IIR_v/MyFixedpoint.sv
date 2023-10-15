`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2023/07/30 10:52:16
// Design Name: 
// Module Name: Fixedpoint
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


package Fixedpoint;
`define max(x,y) x>y?x:y;
`define DEF_FP_ADD(x,y,i0,f0,i1,f1,fr) \
    ((f0)>=(f1))?\
    ((max((i0),(i1))+(f0))'(x)+\
    ( (max((i0),(i1))+(f0))'(y) <<<((f0)-(f1)))\
    )>>> ((f0)-(fr)):\
    (((max((i0),(i1))+(f1))'(x) <<<((f1)-(f0)))+\
    (max ( (i0),(i1))+(f1))'(y)\
    )>>> ((f1)-(fr))
`define DEF_FP_MUL(x,y,i0,f0,i1,f1,fr)\
    (((i0)+(i1)+(f0)+(f1))'(x)*((i0)+(i1)+(f0)+(f1))'(y))>>>((f0)+(f1)-(fr))
endpackage
