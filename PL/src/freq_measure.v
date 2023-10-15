module freq_measure #(
    parameter WIDTH = 16
)(
    input wire signal_in,
    input wire clk_in,
    input wire rst_n,
    input wire[WIDTH-1:0] gate_cnt,
    output wire[WIDTH-1:0] signal_cnt
    );
    reg[WIDTH-1:0] clk_tmp,signal_tmp;
    assign signal_cnt = signal_tmp;
    always@(posedge clk_in, negedge rst_n) begin
        if(rst_n == 0)
            clk_tmp <= 0;
        else if(clk_tmp < gate_cnt)
            clk_tmp <= clk_tmp+1;
        else
            clk_tmp <= clk_tmp;
    end
    
    always@(posedge signal_in, negedge rst_n) begin
        if(rst_n == 0)
            signal_tmp <= 0;
        else if(clk_tmp < gate_cnt)
            signal_tmp <= signal_tmp+1;
        else
            signal_tmp <= signal_tmp;
    end
endmodule
