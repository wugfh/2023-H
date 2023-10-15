//Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
//Copyright 2022-2023 Advanced Micro Devices, Inc. All Rights Reserved.
//--------------------------------------------------------------------------------
//Tool Version: Vivado v.2023.1 (win64) Build 3865809 Sun May  7 15:05:29 MDT 2023
//Date        : Sat Oct 14 10:25:55 2023
//Host        : wuhawuha running 64-bit major release  (build 9200)
//Command     : generate_target simPL_wrapper.bd
//Design      : simPL_wrapper
//Purpose     : IP block netlist
//--------------------------------------------------------------------------------
`timescale 1 ps / 1 ps

module simPL_wrapper
   (clk,
    phase_offset_0,
    rst_n,
    set0,
    set1);
  input clk;
  output [31:0]phase_offset_0;
  input rst_n;
  output set0;
  output set1;

  wire clk;
  wire [31:0]phase_offset_0;
  wire rst_n;
  wire set0;
  wire set1;

  simPL simPL_i
       (.clk(clk),
        .phase_offset_0(phase_offset_0),
        .rst_n(rst_n),
        .set0(set0),
        .set1(set1));
endmodule
