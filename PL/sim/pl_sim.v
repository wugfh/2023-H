`timescale 1ns/1ps

module pl_sim;
    parameter PERIOD1 = 10;
    reg[31:0] DFREQ_WORD = 1342177;
    reg clk = 0, rst_n = 0;
    wire set0,set1;
    wire[31:0] phase_out;
    always #(PERIOD1/2) clk = ~clk;
    
    
   simPL_wrapper sim(
    .phase_offset_0(phase_out),
    .clk(clk),
    .rst_n(rst_n),
    .set0(set0),
    .set1(set1)
    );
    initial begin
        #(PERIOD1*4) rst_n <= 1;
    end
//    integer i = 0;
//    integer step_freq_word = 26843;
//    initial begin
//        #(PERIOD1*6) i = 0;
//        for(i = 0; i < 5000; i=i+1) begin
//        #(PERIOD1*20) DFREQ_WORD <= DFREQ_WORD+step_freq_word;
//        end
//    end
    
    initial begin
        #(PERIOD1*10000) $stop();
        $finish();
    end
    
endmodule
