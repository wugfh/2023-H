module complement_2_true #(
    parameter width = 16
)(
    input[width-1:0] in_complement,
    output[width-1:0] out_true
    );
    assign out_true = in_complement+(1<<(width-1));
endmodule
