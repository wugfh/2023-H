module signed_extend #(
    parameter INPUT_WIDTH = 16,
    parameter OUTPUT_WIDTH = 32
)(
    input wire[INPUT_WIDTH-1:0] din,
    output wire[OUTPUT_WIDTH-1:0] dout
);
    assign dout = {{(OUTPUT_WIDTH-INPUT_WIDTH){din[INPUT_WIDTH-1]}}, din};
endmodule
