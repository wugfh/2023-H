module selector #(
    parameter WIDTH = 14
)(
    input wire[WIDTH-1:0] dds_in,
    input wire[WIDTH-1:0] tri_in,
    input wire which,
    output wire[WIDTH-1:0] signal_out
    );
    assign  signal_out = which ? dds_in:tri_in;
endmodule
