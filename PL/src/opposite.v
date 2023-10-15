module opposite #(
    parameter W = 8
)(
    input wire[W-1:0] in_data,
    output wire[W-1:0] out_data
    );
    assign out_data = ~in_data+1;
endmodule
