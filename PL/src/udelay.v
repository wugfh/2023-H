module udelay #(
    parameter WIDTH = 16
)(
    input wire clk,
    input wire rst_n,
    input wire[WIDTH-1:0] delay_in,
    output wire[WIDTH-1:0] delay_out
    );
    reg[WIDTH-1:0] temp = 0,out_reg = 0;
    assign delay_out = out_reg;
    always @(posedge clk, negedge rst_n) begin
        if(rst_n == 0) begin
            out_reg <= 0;
            temp <= 0;
        end
        else begin
            temp <= delay_in;
            out_reg <= delay_out;
        end
    end
endmodule
