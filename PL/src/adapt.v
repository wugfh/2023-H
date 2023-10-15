module adapt #(
    parameter WIDTH = 16
)(
    input wire signed[WIDTH-1:0] adc_adapt_in,
    output wire signed [WIDTH-1:0] peak
    );
    assign peak = (adc_adapt_in > peak)?adc_adapt_in: peak;
endmodule
