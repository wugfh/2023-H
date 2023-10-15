module extract #(
    parameter W = 16,
    parameter RATE = 2
)(  
    input wire in_clk, 
    input wire rst_n,
    input wire in_valid,
    input wire[W-1:0] high_in,
    output wire[W-1:0] low_out,
    output wire valid
    );
    reg[W-1:0] cnt = 0,valid_reg = 0;
    reg[W-1:0] low_out_reg;
    assign low_out = low_out_reg;
    assign valid = valid_reg;
    always @(posedge in_clk, negedge rst_n) begin
        if(rst_n == 0) begin
            low_out_reg <= 0;
            cnt <= 0;
        end
        else if(cnt == RATE&&in_valid==1) begin
            low_out_reg = high_in;
            valid_reg <= 1;
            cnt <= 0;
        end
        else begin
            low_out_reg <= low_out_reg;
            valid_reg <= 0;
            cnt <= cnt+1;
        end  
    end
endmodule
