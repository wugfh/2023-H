module triangle #(
    parameter WIDTH = 32,
    parameter CNT_MAX = 32'hffffffff
)(
    input wire[31:0] step,
    input wire clk,
    input wire rst_n,
    output wire[13:0] tri_out
    );
    reg[31:0] step_reg=0;
    reg[WIDTH-1:0] cnt=0;
    reg flag = 0;
    reg[31:0] max_cnt0,min_cnt=0;
    assign tri_out = cnt[31:18];
    always @(posedge clk, negedge rst_n) begin
        if(rst_n == 0) begin
            step_reg <= 0;
        end
        else begin
            step_reg <= step;
        end
    end
    always @(posedge clk, negedge rst_n) begin
        if(rst_n == 0) begin
            cnt <= 0;
        end
        else begin
            if(flag == 0) begin
                cnt <= cnt+step_reg;
            end
            else begin
                cnt <= cnt-step_reg;
            end
        end
    end
    always @(posedge clk, negedge rst_n) begin
        if(rst_n == 0) begin
            flag <= 0;
        end
        else begin
            if(cnt > (CNT_MAX-(step_reg<<1))) begin
                flag <= 1;
            end
            else if(cnt < (step_reg<<1)) begin
                flag <= 0;
            end
            else begin
                flag <= flag;
            end
        end
    end
endmodule
