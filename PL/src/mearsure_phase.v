module mearsure_phase #(
    parameter WIDTH = 32
)(
    input wire clk, rst_n,
    input wire set_a, set_b,
    output wire [WIDTH-1:0] phase_offset,
    output wire [WIDTH-1:0] cnt_out
    );
    reg[WIDTH-1:0] cnt, phase_reg;
    reg flag;
    assign phase_offset = phase_reg;
    assign cnt_out = cnt;
    always @(posedge clk, negedge rst_n) begin
       if(rst_n == 0) begin
            cnt <= 0;
       end 
       else if(set_a == 0) begin
            cnt <= 0;
       end
       else if(flag == 1)begin
            cnt <= cnt+1;
       end
       else begin
            cnt <= cnt;
       end
    end
    always @(negedge clk) begin
        if(set_b == 0) begin
            phase_reg <= cnt;
        end
    end
    always @(posedge clk) begin
        if(set_a == 0) begin
            flag <= 1;
        end
        else if(set_b == 0) begin
            flag <= 0;
        end
    end
endmodule
