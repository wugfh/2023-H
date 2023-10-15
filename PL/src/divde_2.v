module divde_2(
    input wire clk,
    input wire rst_n,
    output wire clk_2
    );
    reg clk_2_reg = 0;
    assign clk_2 = clk_2_reg;
    always @(posedge clk, negedge rst_n)begin
        if(rst_n == 0)
            clk_2_reg <= 0;
        else 
            clk_2_reg <= ~clk_2_reg;
    end
endmodule
