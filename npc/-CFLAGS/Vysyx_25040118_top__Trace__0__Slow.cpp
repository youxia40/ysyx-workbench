// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Tracing implementation internals
#include "verilated_fst_c.h"
#include "Vysyx_25040118_top__Syms.h"


VL_ATTR_COLD void Vysyx_25040118_top___024root__trace_init_sub__TOP__0(Vysyx_25040118_top___024root* vlSelf, VerilatedFst* tracep) {
    if (false && vlSelf) {}  // Prevent unused
    Vysyx_25040118_top__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vysyx_25040118_top___024root__trace_init_sub__TOP__0\n"); );
    // Init
    const int c = vlSymsp->__Vm_baseCode;
    // Body
    tracep->declBit(c+59,"clk",-1,FST_VD_INPUT,FST_VT_VCD_WIRE, false,-1);
    tracep->declBit(c+60,"rst_n",-1,FST_VD_INPUT,FST_VT_VCD_WIRE, false,-1);
    tracep->pushNamePrefix("ysyx_25040118_top ");
    tracep->declBit(c+59,"clk",-1,FST_VD_INPUT,FST_VT_VCD_WIRE, false,-1);
    tracep->declBit(c+60,"rst_n",-1,FST_VD_INPUT,FST_VT_VCD_WIRE, false,-1);
    tracep->declBit(c+61,"rst",-1, FST_VD_IMPLICIT,FST_VT_VCD_WIRE, false,-1);
    tracep->declBus(c+1,"pc",-1, FST_VD_IMPLICIT,FST_VT_VCD_WIRE, false,-1, 31,0);
    tracep->declBus(c+2,"next_pc",-1, FST_VD_IMPLICIT,FST_VT_VCD_WIRE, false,-1, 31,0);
    tracep->declBus(c+3,"inst",-1, FST_VD_IMPLICIT,FST_VT_VCD_WIRE, false,-1, 31,0);
    tracep->declBus(c+4,"rd",-1, FST_VD_IMPLICIT,FST_VT_VCD_WIRE, false,-1, 4,0);
    tracep->declBus(c+5,"rs1",-1, FST_VD_IMPLICIT,FST_VT_VCD_WIRE, false,-1, 4,0);
    tracep->declBus(c+6,"rs2",-1, FST_VD_IMPLICIT,FST_VT_VCD_WIRE, false,-1, 4,0);
    tracep->declBus(c+62,"imm",-1, FST_VD_IMPLICIT,FST_VT_VCD_WIRE, false,-1, 31,0);
    tracep->declBus(c+7,"opcode",-1, FST_VD_IMPLICIT,FST_VT_VCD_WIRE, false,-1, 6,0);
    tracep->declBit(c+8,"ebreak",-1, FST_VD_IMPLICIT,FST_VT_VCD_WIRE, false,-1);
    tracep->declBus(c+9,"alu_ctrl",-1, FST_VD_IMPLICIT,FST_VT_VCD_WIRE, false,-1, 4,0);
    tracep->declBus(c+63,"exu_result",-1, FST_VD_IMPLICIT,FST_VT_VCD_WIRE, false,-1, 31,0);
    tracep->declBus(c+10,"reg_src1",-1, FST_VD_IMPLICIT,FST_VT_VCD_WIRE, false,-1, 31,0);
    tracep->declBus(c+11,"reg_src2",-1, FST_VD_IMPLICIT,FST_VT_VCD_WIRE, false,-1, 31,0);
    tracep->declBit(c+12,"mem_wren",-1, FST_VD_IMPLICIT,FST_VT_VCD_WIRE, false,-1);
    tracep->declBus(c+13,"mem_addr",-1, FST_VD_IMPLICIT,FST_VT_VCD_WIRE, false,-1, 31,0);
    tracep->declBus(c+11,"mem_wdata",-1, FST_VD_IMPLICIT,FST_VT_VCD_WIRE, false,-1, 31,0);
    tracep->declBus(c+14,"mem_wmask",-1, FST_VD_IMPLICIT,FST_VT_VCD_WIRE, false,-1, 3,0);
    tracep->declBus(c+15,"data_rdata",-1, FST_VD_IMPLICIT,FST_VT_VCD_WIRE, false,-1, 31,0);
    tracep->pushNamePrefix("exu ");
    tracep->declBit(c+59,"clk",-1,FST_VD_INPUT,FST_VT_VCD_WIRE, false,-1);
    tracep->declBit(c+61,"rst",-1,FST_VD_INPUT,FST_VT_VCD_WIRE, false,-1);
    tracep->declBus(c+1,"pc",-1,FST_VD_INPUT,FST_VT_VCD_WIRE, false,-1, 31,0);
    tracep->declBus(c+3,"inst",-1,FST_VD_INPUT,FST_VT_VCD_WIRE, false,-1, 31,0);
    tracep->declBus(c+10,"src1",-1,FST_VD_INPUT,FST_VT_VCD_WIRE, false,-1, 31,0);
    tracep->declBus(c+11,"src2",-1,FST_VD_INPUT,FST_VT_VCD_WIRE, false,-1, 31,0);
    tracep->declBus(c+62,"imm",-1,FST_VD_INPUT,FST_VT_VCD_WIRE, false,-1, 31,0);
    tracep->declBus(c+9,"alu_ctrl",-1,FST_VD_INPUT,FST_VT_VCD_WIRE, false,-1, 4,0);
    tracep->declBus(c+63,"result",-1,FST_VD_OUTPUT,FST_VT_VCD_WIRE, false,-1, 31,0);
    tracep->declBus(c+2,"next_pc",-1,FST_VD_OUTPUT,FST_VT_VCD_WIRE, false,-1, 31,0);
    tracep->declBit(c+12,"mem_wren",-1,FST_VD_OUTPUT,FST_VT_VCD_WIRE, false,-1);
    tracep->declBus(c+13,"mem_addr",-1,FST_VD_OUTPUT,FST_VT_VCD_WIRE, false,-1, 31,0);
    tracep->declBus(c+11,"mem_wdata",-1,FST_VD_OUTPUT,FST_VT_VCD_WIRE, false,-1, 31,0);
    tracep->declBus(c+14,"mem_wmask",-1,FST_VD_OUTPUT,FST_VT_VCD_WIRE, false,-1, 3,0);
    tracep->declBit(c+64,"branch_taken",-1,FST_VD_OUTPUT,FST_VT_VCD_WIRE, false,-1);
    tracep->declBus(c+16,"alu_op",-1, FST_VD_IMPLICIT,FST_VT_VCD_WIRE, false,-1, 1,0);
    tracep->declBus(c+17,"funct3",-1, FST_VD_IMPLICIT,FST_VT_VCD_WIRE, false,-1, 2,0);
    tracep->declBus(c+7,"opcode",-1, FST_VD_IMPLICIT,FST_VT_VCD_WIRE, false,-1, 6,0);
    tracep->declBus(c+18,"alu_result",-1, FST_VD_IMPLICIT,FST_VT_SV_LOGIC, false,-1, 31,0);
    tracep->declBus(c+19,"src1_sign_ext",-1, FST_VD_IMPLICIT,FST_VT_SV_LOGIC, false,-1, 31,0);
    tracep->declBit(c+20,"branch",-1, FST_VD_IMPLICIT,FST_VT_SV_LOGIC, false,-1);
    tracep->popNamePrefix(1);
    tracep->pushNamePrefix("idu ");
    tracep->declBus(c+3,"inst",-1,FST_VD_INPUT,FST_VT_VCD_WIRE, false,-1, 31,0);
    tracep->declBus(c+1,"pc",-1,FST_VD_INPUT,FST_VT_VCD_WIRE, false,-1, 31,0);
    tracep->declBus(c+4,"rd",-1,FST_VD_OUTPUT,FST_VT_VCD_WIRE, false,-1, 4,0);
    tracep->declBus(c+5,"rs1",-1,FST_VD_OUTPUT,FST_VT_VCD_WIRE, false,-1, 4,0);
    tracep->declBus(c+6,"rs2",-1,FST_VD_OUTPUT,FST_VT_VCD_WIRE, false,-1, 4,0);
    tracep->declBus(c+62,"imm",-1,FST_VD_OUTPUT,FST_VT_VCD_WIRE, false,-1, 31,0);
    tracep->declBus(c+7,"opcode",-1,FST_VD_OUTPUT,FST_VT_VCD_WIRE, false,-1, 6,0);
    tracep->declBit(c+8,"ebreak",-1,FST_VD_OUTPUT,FST_VT_VCD_WIRE, false,-1);
    tracep->declBus(c+9,"alu_ctrl",-1,FST_VD_OUTPUT,FST_VT_VCD_WIRE, false,-1, 4,0);
    tracep->declBus(c+19,"i_imm",-1, FST_VD_IMPLICIT,FST_VT_VCD_WIRE, false,-1, 31,0);
    tracep->declBus(c+21,"s_imm",-1, FST_VD_IMPLICIT,FST_VT_VCD_WIRE, false,-1, 31,0);
    tracep->declBus(c+22,"b_imm",-1, FST_VD_IMPLICIT,FST_VT_VCD_WIRE, false,-1, 31,0);
    tracep->declBus(c+23,"u_imm",-1, FST_VD_IMPLICIT,FST_VT_VCD_WIRE, false,-1, 31,0);
    tracep->declBus(c+24,"j_imm",-1, FST_VD_IMPLICIT,FST_VT_VCD_WIRE, false,-1, 31,0);
    tracep->popNamePrefix(1);
    tracep->pushNamePrefix("ifu ");
    tracep->declBit(c+59,"clk",-1,FST_VD_INPUT,FST_VT_VCD_WIRE, false,-1);
    tracep->declBit(c+61,"rst",-1,FST_VD_INPUT,FST_VT_VCD_WIRE, false,-1);
    tracep->declBus(c+2,"next_pc",-1,FST_VD_INPUT,FST_VT_VCD_WIRE, false,-1, 31,0);
    tracep->declBus(c+1,"pc",-1,FST_VD_OUTPUT,FST_VT_VCD_WIRE, false,-1, 31,0);
    tracep->declBus(c+3,"inst",-1,FST_VD_OUTPUT,FST_VT_VCD_WIRE, false,-1, 31,0);
    tracep->popNamePrefix(1);
    tracep->pushNamePrefix("mem ");
    tracep->declBit(c+59,"clk",-1,FST_VD_INPUT,FST_VT_VCD_WIRE, false,-1);
    tracep->declBit(c+61,"rst",-1,FST_VD_INPUT,FST_VT_VCD_WIRE, false,-1);
    tracep->declBus(c+1,"inst_addr",-1,FST_VD_INPUT,FST_VT_VCD_WIRE, false,-1, 31,0);
    tracep->declBus(c+3,"inst_data",-1,FST_VD_OUTPUT,FST_VT_VCD_WIRE, false,-1, 31,0);
    tracep->declBit(c+12,"data_we",-1,FST_VD_INPUT,FST_VT_VCD_WIRE, false,-1);
    tracep->declBus(c+13,"data_addr",-1,FST_VD_INPUT,FST_VT_VCD_WIRE, false,-1, 31,0);
    tracep->declBus(c+11,"data_wdata",-1,FST_VD_INPUT,FST_VT_VCD_WIRE, false,-1, 31,0);
    tracep->declBus(c+14,"data_wmask",-1,FST_VD_INPUT,FST_VT_VCD_WIRE, false,-1, 3,0);
    tracep->declBus(c+15,"data_rdata",-1,FST_VD_OUTPUT,FST_VT_VCD_WIRE, false,-1, 31,0);
    tracep->popNamePrefix(1);
    tracep->pushNamePrefix("reg_file ");
    tracep->declBit(c+59,"clk",-1,FST_VD_INPUT,FST_VT_VCD_WIRE, false,-1);
    tracep->declBit(c+61,"rst",-1,FST_VD_INPUT,FST_VT_VCD_WIRE, false,-1);
    tracep->declBus(c+4,"waddr",-1,FST_VD_INPUT,FST_VT_VCD_WIRE, false,-1, 4,0);
    tracep->declBus(c+63,"wdata",-1,FST_VD_INPUT,FST_VT_VCD_WIRE, false,-1, 31,0);
    tracep->declBit(c+25,"wren",-1,FST_VD_INPUT,FST_VT_VCD_WIRE, false,-1);
    tracep->declBus(c+5,"raddr1",-1,FST_VD_INPUT,FST_VT_VCD_WIRE, false,-1, 4,0);
    tracep->declBus(c+10,"rdata1",-1,FST_VD_OUTPUT,FST_VT_VCD_WIRE, false,-1, 31,0);
    tracep->declBus(c+6,"raddr2",-1,FST_VD_INPUT,FST_VT_VCD_WIRE, false,-1, 4,0);
    tracep->declBus(c+11,"rdata2",-1,FST_VD_OUTPUT,FST_VT_VCD_WIRE, false,-1, 31,0);
    for (int i = 0; i < 32; ++i) {
        tracep->declBus(c+26+i*1,"reg_file",-1, FST_VD_IMPLICIT,FST_VT_SV_LOGIC, true,(i+0), 31,0);
    }
    tracep->pushNamePrefix("unnamedblk1 ");
    tracep->declBus(c+58,"i",-1, FST_VD_IMPLICIT,FST_VT_SV_INT, false,-1, 31,0);
    tracep->popNamePrefix(3);
}

VL_ATTR_COLD void Vysyx_25040118_top___024root__trace_init_top(Vysyx_25040118_top___024root* vlSelf, VerilatedFst* tracep) {
    if (false && vlSelf) {}  // Prevent unused
    Vysyx_25040118_top__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vysyx_25040118_top___024root__trace_init_top\n"); );
    // Body
    Vysyx_25040118_top___024root__trace_init_sub__TOP__0(vlSelf, tracep);
}

VL_ATTR_COLD void Vysyx_25040118_top___024root__trace_full_top_0(void* voidSelf, VerilatedFst::Buffer* bufp);
void Vysyx_25040118_top___024root__trace_chg_top_0(void* voidSelf, VerilatedFst::Buffer* bufp);
void Vysyx_25040118_top___024root__trace_cleanup(void* voidSelf, VerilatedFst* /*unused*/);

VL_ATTR_COLD void Vysyx_25040118_top___024root__trace_register(Vysyx_25040118_top___024root* vlSelf, VerilatedFst* tracep) {
    if (false && vlSelf) {}  // Prevent unused
    Vysyx_25040118_top__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vysyx_25040118_top___024root__trace_register\n"); );
    // Body
    tracep->addFullCb(&Vysyx_25040118_top___024root__trace_full_top_0, vlSelf);
    tracep->addChgCb(&Vysyx_25040118_top___024root__trace_chg_top_0, vlSelf);
    tracep->addCleanupCb(&Vysyx_25040118_top___024root__trace_cleanup, vlSelf);
}

VL_ATTR_COLD void Vysyx_25040118_top___024root__trace_full_sub_0(Vysyx_25040118_top___024root* vlSelf, VerilatedFst::Buffer* bufp);

VL_ATTR_COLD void Vysyx_25040118_top___024root__trace_full_top_0(void* voidSelf, VerilatedFst::Buffer* bufp) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vysyx_25040118_top___024root__trace_full_top_0\n"); );
    // Init
    Vysyx_25040118_top___024root* const __restrict vlSelf VL_ATTR_UNUSED = static_cast<Vysyx_25040118_top___024root*>(voidSelf);
    Vysyx_25040118_top__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    // Body
    Vysyx_25040118_top___024root__trace_full_sub_0((&vlSymsp->TOP), bufp);
}

VL_ATTR_COLD void Vysyx_25040118_top___024root__trace_full_sub_0(Vysyx_25040118_top___024root* vlSelf, VerilatedFst::Buffer* bufp) {
    if (false && vlSelf) {}  // Prevent unused
    Vysyx_25040118_top__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vysyx_25040118_top___024root__trace_full_sub_0\n"); );
    // Init
    uint32_t* const oldp VL_ATTR_UNUSED = bufp->oldp(vlSymsp->__Vm_baseCode);
    // Body
    bufp->fullIData(oldp+1,(vlSelf->ysyx_25040118_top__DOT__pc),32);
    bufp->fullIData(oldp+2,(((0x6fU == (0x7fU & vlSelf->ysyx_25040118_top__DOT__inst))
                              ? vlSelf->ysyx_25040118_top__DOT__exu__DOT____VdfgTmp_h8a6dd40b__0
                              : ((0x67U == (0x7fU & vlSelf->ysyx_25040118_top__DOT__inst))
                                  ? (0xfffffffeU & vlSelf->ysyx_25040118_top__DOT__exu__DOT____VdfgTmp_h623bd169__0)
                                  : (((0x63U == (0x7fU 
                                                 & vlSelf->ysyx_25040118_top__DOT__inst))
                                       ? ((0U == (7U 
                                                  & (vlSelf->ysyx_25040118_top__DOT__inst 
                                                     >> 0xcU)))
                                           ? (vlSelf->ysyx_25040118_top__DOT__reg_src1 
                                              == vlSelf->ysyx_25040118_top__DOT__reg_src2)
                                           : (IData)(
                                                     ((0x1000U 
                                                       == 
                                                       (0x7000U 
                                                        & vlSelf->ysyx_25040118_top__DOT__inst)) 
                                                      & (vlSelf->ysyx_25040118_top__DOT__reg_src1 
                                                         != vlSelf->ysyx_25040118_top__DOT__reg_src2))))
                                       : ((0x63U == 
                                           (0x7fU & vlSelf->ysyx_25040118_top__DOT__inst)) 
                                          & ((4U == 
                                              (7U & 
                                               (vlSelf->ysyx_25040118_top__DOT__inst 
                                                >> 0xcU)))
                                              ? VL_LTS_III(32, vlSelf->ysyx_25040118_top__DOT__reg_src1, vlSelf->ysyx_25040118_top__DOT__reg_src2)
                                              : ((5U 
                                                  == 
                                                  (7U 
                                                   & (vlSelf->ysyx_25040118_top__DOT__inst 
                                                      >> 0xcU)))
                                                  ? 
                                                 VL_GTES_III(32, vlSelf->ysyx_25040118_top__DOT__reg_src1, vlSelf->ysyx_25040118_top__DOT__reg_src2)
                                                  : 
                                                 ((2U 
                                                   == 
                                                   (7U 
                                                    & (vlSelf->ysyx_25040118_top__DOT__inst 
                                                       >> 0xcU)))
                                                   ? 
                                                  (vlSelf->ysyx_25040118_top__DOT__reg_src1 
                                                   < vlSelf->ysyx_25040118_top__DOT__reg_src2)
                                                   : (IData)(
                                                             ((0x3000U 
                                                               == 
                                                               (0x7000U 
                                                                & vlSelf->ysyx_25040118_top__DOT__inst)) 
                                                              & (vlSelf->ysyx_25040118_top__DOT__reg_src1 
                                                                 >= vlSelf->ysyx_25040118_top__DOT__reg_src2))))))))
                                      ? vlSelf->ysyx_25040118_top__DOT__exu__DOT____VdfgTmp_h8a6dd40b__0
                                      : ((IData)(4U) 
                                         + vlSelf->ysyx_25040118_top__DOT__pc))))),32);
    bufp->fullIData(oldp+3,(vlSelf->ysyx_25040118_top__DOT__inst),32);
    bufp->fullCData(oldp+4,((0x1fU & (vlSelf->ysyx_25040118_top__DOT__inst 
                                      >> 7U))),5);
    bufp->fullCData(oldp+5,((0x1fU & (vlSelf->ysyx_25040118_top__DOT__inst 
                                      >> 0xfU))),5);
    bufp->fullCData(oldp+6,((0x1fU & (vlSelf->ysyx_25040118_top__DOT__inst 
                                      >> 0x14U))),5);
    bufp->fullCData(oldp+7,((0x7fU & vlSelf->ysyx_25040118_top__DOT__inst)),7);
    bufp->fullBit(oldp+8,((0x100073U == vlSelf->ysyx_25040118_top__DOT__inst)));
    bufp->fullCData(oldp+9,(vlSelf->ysyx_25040118_top__DOT__alu_ctrl),5);
    bufp->fullIData(oldp+10,(vlSelf->ysyx_25040118_top__DOT__reg_src1),32);
    bufp->fullIData(oldp+11,(vlSelf->ysyx_25040118_top__DOT__reg_src2),32);
    bufp->fullBit(oldp+12,((0x23U == (0x7fU & vlSelf->ysyx_25040118_top__DOT__inst))));
    bufp->fullIData(oldp+13,(vlSelf->ysyx_25040118_top__DOT__mem_addr),32);
    bufp->fullCData(oldp+14,(vlSelf->ysyx_25040118_top__DOT__mem_wmask),4);
    bufp->fullIData(oldp+15,(vlSelf->ysyx_25040118_top__DOT__mem__DOT__dmem
                             [(((3U == (0x7fU & vlSelf->ysyx_25040118_top__DOT__inst)) 
                                | (0x23U == (0x7fU 
                                             & vlSelf->ysyx_25040118_top__DOT__inst)))
                                ? (0x3ffU & (vlSelf->ysyx_25040118_top__DOT__exu__DOT____VdfgTmp_h623bd169__0 
                                             >> 2U))
                                : 0U)]),32);
    bufp->fullCData(oldp+16,((3U & (IData)(vlSelf->ysyx_25040118_top__DOT__alu_ctrl))),2);
    bufp->fullCData(oldp+17,((7U & (vlSelf->ysyx_25040118_top__DOT__inst 
                                    >> 0xcU))),3);
    bufp->fullIData(oldp+18,(vlSelf->ysyx_25040118_top__DOT__exu__DOT__alu_result),32);
    bufp->fullIData(oldp+19,((((- (IData)((vlSelf->ysyx_25040118_top__DOT__inst 
                                           >> 0x1fU))) 
                               << 0xcU) | (vlSelf->ysyx_25040118_top__DOT__inst 
                                           >> 0x14U))),32);
    bufp->fullBit(oldp+20,(((0x63U == (0x7fU & vlSelf->ysyx_25040118_top__DOT__inst))
                             ? ((0U == (7U & (vlSelf->ysyx_25040118_top__DOT__inst 
                                              >> 0xcU)))
                                 ? (vlSelf->ysyx_25040118_top__DOT__reg_src1 
                                    == vlSelf->ysyx_25040118_top__DOT__reg_src2)
                                 : (IData)(((0x1000U 
                                             == (0x7000U 
                                                 & vlSelf->ysyx_25040118_top__DOT__inst)) 
                                            & (vlSelf->ysyx_25040118_top__DOT__reg_src1 
                                               != vlSelf->ysyx_25040118_top__DOT__reg_src2))))
                             : ((0x63U == (0x7fU & vlSelf->ysyx_25040118_top__DOT__inst)) 
                                & ((4U == (7U & (vlSelf->ysyx_25040118_top__DOT__inst 
                                                 >> 0xcU)))
                                    ? VL_LTS_III(32, vlSelf->ysyx_25040118_top__DOT__reg_src1, vlSelf->ysyx_25040118_top__DOT__reg_src2)
                                    : ((5U == (7U & 
                                               (vlSelf->ysyx_25040118_top__DOT__inst 
                                                >> 0xcU)))
                                        ? VL_GTES_III(32, vlSelf->ysyx_25040118_top__DOT__reg_src1, vlSelf->ysyx_25040118_top__DOT__reg_src2)
                                        : ((2U == (7U 
                                                   & (vlSelf->ysyx_25040118_top__DOT__inst 
                                                      >> 0xcU)))
                                            ? (vlSelf->ysyx_25040118_top__DOT__reg_src1 
                                               < vlSelf->ysyx_25040118_top__DOT__reg_src2)
                                            : (IData)(
                                                      ((0x3000U 
                                                        == 
                                                        (0x7000U 
                                                         & vlSelf->ysyx_25040118_top__DOT__inst)) 
                                                       & (vlSelf->ysyx_25040118_top__DOT__reg_src1 
                                                          >= vlSelf->ysyx_25040118_top__DOT__reg_src2))))))))));
    bufp->fullIData(oldp+21,((((- (IData)((vlSelf->ysyx_25040118_top__DOT__inst 
                                           >> 0x1fU))) 
                               << 0xcU) | ((0xfe0U 
                                            & (vlSelf->ysyx_25040118_top__DOT__inst 
                                               >> 0x14U)) 
                                           | (0x1fU 
                                              & (vlSelf->ysyx_25040118_top__DOT__inst 
                                                 >> 7U))))),32);
    bufp->fullIData(oldp+22,((((- (IData)((vlSelf->ysyx_25040118_top__DOT__inst 
                                           >> 0x1fU))) 
                               << 0xcU) | ((0x800U 
                                            & (vlSelf->ysyx_25040118_top__DOT__inst 
                                               << 4U)) 
                                           | ((0x7e0U 
                                               & (vlSelf->ysyx_25040118_top__DOT__inst 
                                                  >> 0x14U)) 
                                              | (0x1eU 
                                                 & (vlSelf->ysyx_25040118_top__DOT__inst 
                                                    >> 7U)))))),32);
    bufp->fullIData(oldp+23,((0xfffff000U & vlSelf->ysyx_25040118_top__DOT__inst)),32);
    bufp->fullIData(oldp+24,((((- (IData)((vlSelf->ysyx_25040118_top__DOT__inst 
                                           >> 0x1fU))) 
                               << 0x14U) | ((0xff000U 
                                             & vlSelf->ysyx_25040118_top__DOT__inst) 
                                            | ((0x800U 
                                                & (vlSelf->ysyx_25040118_top__DOT__inst 
                                                   >> 9U)) 
                                               | (0x7feU 
                                                  & (vlSelf->ysyx_25040118_top__DOT__inst 
                                                     >> 0x14U)))))),32);
    bufp->fullBit(oldp+25,(((0x63U != (0x7fU & vlSelf->ysyx_25040118_top__DOT__inst)) 
                            & ((0x23U != (0x7fU & vlSelf->ysyx_25040118_top__DOT__inst)) 
                               & (0U != (0x1fU & (vlSelf->ysyx_25040118_top__DOT__inst 
                                                  >> 7U)))))));
    bufp->fullIData(oldp+26,(vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file[0]),32);
    bufp->fullIData(oldp+27,(vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file[1]),32);
    bufp->fullIData(oldp+28,(vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file[2]),32);
    bufp->fullIData(oldp+29,(vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file[3]),32);
    bufp->fullIData(oldp+30,(vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file[4]),32);
    bufp->fullIData(oldp+31,(vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file[5]),32);
    bufp->fullIData(oldp+32,(vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file[6]),32);
    bufp->fullIData(oldp+33,(vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file[7]),32);
    bufp->fullIData(oldp+34,(vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file[8]),32);
    bufp->fullIData(oldp+35,(vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file[9]),32);
    bufp->fullIData(oldp+36,(vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file[10]),32);
    bufp->fullIData(oldp+37,(vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file[11]),32);
    bufp->fullIData(oldp+38,(vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file[12]),32);
    bufp->fullIData(oldp+39,(vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file[13]),32);
    bufp->fullIData(oldp+40,(vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file[14]),32);
    bufp->fullIData(oldp+41,(vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file[15]),32);
    bufp->fullIData(oldp+42,(vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file[16]),32);
    bufp->fullIData(oldp+43,(vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file[17]),32);
    bufp->fullIData(oldp+44,(vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file[18]),32);
    bufp->fullIData(oldp+45,(vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file[19]),32);
    bufp->fullIData(oldp+46,(vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file[20]),32);
    bufp->fullIData(oldp+47,(vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file[21]),32);
    bufp->fullIData(oldp+48,(vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file[22]),32);
    bufp->fullIData(oldp+49,(vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file[23]),32);
    bufp->fullIData(oldp+50,(vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file[24]),32);
    bufp->fullIData(oldp+51,(vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file[25]),32);
    bufp->fullIData(oldp+52,(vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file[26]),32);
    bufp->fullIData(oldp+53,(vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file[27]),32);
    bufp->fullIData(oldp+54,(vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file[28]),32);
    bufp->fullIData(oldp+55,(vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file[29]),32);
    bufp->fullIData(oldp+56,(vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file[30]),32);
    bufp->fullIData(oldp+57,(vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file[31]),32);
    bufp->fullIData(oldp+58,(vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__unnamedblk1__DOT__i),32);
    bufp->fullBit(oldp+59,(vlSelf->clk));
    bufp->fullBit(oldp+60,(vlSelf->rst_n));
    bufp->fullBit(oldp+61,((1U & (~ (IData)(vlSelf->rst_n)))));
    bufp->fullIData(oldp+62,(vlSelf->ysyx_25040118_top__DOT__imm),32);
    bufp->fullIData(oldp+63,(vlSelf->ysyx_25040118_top__DOT__exu_result),32);
    bufp->fullBit(oldp+64,(vlSelf->ysyx_25040118_top__DOT__exu__DOT__branch_taken));
}
