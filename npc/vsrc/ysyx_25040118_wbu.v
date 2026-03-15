module ysyx_25040118_wbu (
    input  [4:0]  rd,
    input         is_load,
    input         is_store,
    input         is_branch,
    input         is_system,
    input         is_csrrw,
    input         is_csrrs,
    input         is_csrrc,
    input         is_csrrwi,
    input         is_csrrsi,
    input         is_csrrci,
    input  [31:0] exu_result,
    input  [31:0] lsu_data,
    output [31:0] wb_data,
    output        wb_wen
);

    wire is_csr_any = is_csrrw | is_csrrs | is_csrrc |
                      is_csrrwi | is_csrrsi | is_csrrci;

    //寄存器写使能:rd非x0且不是store/branch;system仅CSR类允许写回
    assign wb_wen = (|rd) && !is_store && !is_branch && (!is_system || is_csr_any);

    //load在WBU统一收敛
    assign wb_data = is_load ? lsu_data : exu_result;

endmodule
