module chufa #(
    parameter WIDTH = 16
)(
    input wire signed [WIDTH-1:0] adc_in,
    input wire clk,
    input wire rst_n,
    output wire set
    );
    reg signed [WIDTH-1:0] delay_1, delay_2;
    reg set_reg;
    reg[1:0] status;
    integer flag;
    wire signed[WIDTH-1:0] lower, upper;
    wire tmp;
    assign tmp = (delay_1 >= 0 && delay_2 < 0)?0:1;
    assign lower = tmp==0?-20:-5;
    assign upper = tmp==1?20:5;
    assign set = set_reg;
    always @(posedge clk, negedge rst_n) begin
        if(rst_n==0) flag <= 0;
        else if(flag < 5) flag <= flag+1;
        else flag <= 0;
    end
    always @(posedge clk, negedge rst_n) begin
        if(rst_n == 0) begin
            delay_1 <= 0;
            delay_2 <= 0;
        end
        else begin
            delay_1 <= adc_in;
            delay_2 <= delay_1;
        end
    end
    always @(posedge clk, negedge rst_n ) begin
        if(rst_n == 0) begin
            set_reg <= 0;
        end
        else begin
            if(tmp == 0 && status == 0) begin
                set_reg <= 0;
            end 
            else begin
                set_reg <= 1;
            end
        end
    end
    always @(posedge clk, negedge rst_n) begin
        if(rst_n == 0) begin
            status <= 0;
        end
        else begin
            if(tmp == 0) status <= 1;
            else if(tmp == 1) status <= 0;
        end
    end
endmodule
