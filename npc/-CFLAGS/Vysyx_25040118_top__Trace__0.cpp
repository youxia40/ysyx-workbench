// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Tracing implementation internals
#include "verilated_fst_c.h"
#include "Vysyx_25040118_top__Syms.h"


void Vysyx_25040118_top___024root__trace_chg_sub_0(Vysyx_25040118_top___024root* vlSelf, VerilatedFst::Buffer* bufp);

void Vysyx_25040118_top___024root__trace_chg_top_0(void* voidSelf, VerilatedFst::Buffer* bufp) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vysyx_25040118_top___024root__trace_chg_top_0\n"); );
    // Init
    Vysyx_25040118_top___024root* const __restrict vlSelf VL_ATTR_UNUSED = static_cast<Vysyx_25040118_top___024root*>(voidSelf);
    Vysyx_25040118_top__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    if (VL_UNLIKELY(!vlSymsp->__Vm_activity)) return;
    // Body
    Vysyx_25040118_top___024root__trace_chg_sub_0((&vlSymsp->TOP), bufp);
}

void Vysyx_25040118_top___024root__trace_chg_sub_0(Vysyx_25040118_top___024root* vlSelf, VerilatedFst::Buffer* bufp) {
    if (false && vlSelf) {}  // Prevent unused
    Vysyx_25040118_top__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vysyx_25040118_top___024root__trace_chg_sub_0\n"); );
    // Init
    uint32_t* const oldp VL_ATTR_UNUSED = bufp->oldp(vlSymsp->__Vm_baseCode + 1);
    // Body
    if (VL_UNLIKELY(vlSelf->__Vm_traceActivity[1U])) {
        bufp->chgIData(oldp+0,(vlSelf->ysyx_25040118_top__DOT__pc),32);
        bufp->chgIData(oldp+1,(((0x6fU == (0x7fU & vlSelf->ysyx_25040118_top__DOT__inst))
                                 ? vlSelf->ysyx_25040118_top__DOT__exu__DOT____VdfgTmp_h8a6dd40b__0
                                 : ((0x67U == (0x7fU 
                                               & vlSelf->ysyx_25040118_top__DOT__inst))
                                     ? (0xfffffffeU 
                                        & vlSelf->ysyx_25040118_top__DOT__exu__DOT____VdfgTmp_h623bd169__0)
                                     : (((0x63U == 
                                          (0x7fU & vlSelf->ysyx_25040118_top__DOT__inst))
                                          ? ((0U == 
                                              (7U & 
                                               (vlSelf->ysyx_25040118_top__DOT__inst 
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
                                          : ((0x63U 
                                              == (0x7fU 
                                                  & vlSelf->ysyx_25040118_top__DOT__inst)) 
                                             & ((4U 
                                                 == 
                                                 (7U 
                                                  & (vlSelf->ysyx_25040118_top__DOT__inst 
                                                     >> 0xcU)))
                                                 ? 
                                                VL_LTS_III(32, vlSelf->ysyx_25040118_top__DOT__reg_src1, vlSelf->ysyx_25040118_top__DOT__reg_src2)
                                                 : 
                                                ((5U 
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
        bufp->chgIData(oldp+2,(vlSelf->ysyx_25040118_top__DOT__inst),32);
        bufp->chgCData(oldp+3,((0x1fU & (vlSelf->ysyx_25040118_top__DOT__inst 
                                         >> 7U))),5);
        bufp->chgCData(oldp+4,((0x1fU & (vlSelf->ysyx_25040118_top__DOT__inst 
                                         >> 0xfU))),5);
        bufp->chgCData(oldp+5,((0x1fU & (vlSelf->ysyx_25040118_top__DOT__inst 
                                         >> 0x14U))),5);
        bufp->chgCData(oldp+6,((0x7fU & vlSelf->ysyx_25040118_top__DOT__inst)),7);
        bufp->chgBit(oldp+7,((0x100073U == vlSelf->ysyx_25040118_top__DOT__inst)));
        bufp->chgCData(oldp+8,(vlSelf->ysyx_25040118_top__DOT__alu_ctrl),5);
        bufp->chgIData(oldp+9,(vlSelf->ysyx_25040118_top__DOT__reg_src1),32);
        bufp->chgIData(oldp+10,(vlSelf->ysyx_25040118_top__DOT__reg_src2),32);
        bufp->chgBit(oldp+11,((0x23U == (0x7fU & vlSelf->ysyx_25040118_top__DOT__inst))));
        bufp->chgIData(oldp+12,(vlSelf->ysyx_25040118_top__DOT__mem_addr),32);
        bufp->chgCData(oldp+13,(vlSelf->ysyx_25040118_top__DOT__mem_wmask),4);
        bufp->chgIData(oldp+14,(vlSelf->ysyx_25040118_top__DOT__mem__DOT__dmem
                                [(((3U == (0x7fU & vlSelf->ysyx_25040118_top__DOT__inst)) 
                                   | (0x23U == (0x7fU 
                                                & vlSelf->ysyx_25040118_top__DOT__inst)))
                                   ? (0x3ffU & (vlSelf->ysyx_25040118_top__DOT__exu__DOT____VdfgTmp_h623bd169__0 
                                                >> 2U))
                                   : 0U)]),32);
        bufp->chgCData(oldp+15,((3U & (IData)(vlSelf->ysyx_25040118_top__DOT__alu_ctrl))),2);
        bufp->chgCData(oldp+16,((7U & (vlSelf->ysyx_25040118_top__DOT__inst 
                                       >> 0xcU))),3);
        bufp->chgIData(oldp+17,(vlSelf->ysyx_25040118_top__DOT__exu__DOT__alu_result),32);
        bufp->chgIData(oldp+18,((((- (IData)((vlSelf->ysyx_25040118_top__DOT__inst 
                                              >> 0x1fU))) 
                                  << 0xcU) | (vlSelf->ysyx_25040118_top__DOT__inst 
                                              >> 0x14U))),32);
        bufp->chgBit(oldp+19,(((0x63U == (0x7fU & vlSelf->ysyx_25040118_top__DOT__inst))
                                ? ((0U == (7U & (vlSelf->ysyx_25040118_top__DOT__inst 
                                                 >> 0xcU)))
                                    ? (vlSelf->ysyx_25040118_top__DOT__reg_src1 
                                       == vlSelf->ysyx_25040118_top__DOT__reg_src2)
                                    : (IData)(((0x1000U 
                                                == 
                                                (0x7000U 
                                                 & vlSelf->ysyx_25040118_top__DOT__inst)) 
                                               & (vlSelf->ysyx_25040118_top__DOT__reg_src1 
                                                  != vlSelf->ysyx_25040118_top__DOT__reg_src2))))
                                : ((0x63U == (0x7fU 
                                              & vlSelf->ysyx_25040118_top__DOT__inst)) 
                                   & ((4U == (7U & 
                                              (vlSelf->ysyx_25040118_top__DOT__inst 
                                               >> 0xcU)))
                                       ? VL_LTS_III(32, vlSelf->ysyx_25040118_top__DOT__reg_src1, vlSelf->ysyx_25040118_top__DOT__reg_src2)
                                       : ((5U == (7U 
                                                  & (vlSelf->ysyx_25040118_top__DOT__inst 
                                                     >> 0xcU)))
                                           ? VL_GTES_III(32, vlSelf->ysyx_25040118_top__DOT__reg_src1, vlSelf->ysyx_25040118_top__DOT__reg_src2)
                                           : ((2U == 
                                               (7U 
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
        bufp->chgIData(oldp+20,((((- (IData)((vlSelf->ysyx_25040118_top__DOT__inst 
                                              >> 0x1fU))) 
                                  << 0xcU) | ((0xfe0U 
                                               & (vlSelf->ysyx_25040118_top__DOT__inst 
                                                  >> 0x14U)) 
                                              | (0x1fU 
                                                 & (vlSelf->ysyx_25040118_top__DOT__inst 
                                                    >> 7U))))),32);
        bufp->chgIData(oldp+21,((((- (IData)((vlSelf->ysyx_25040118_top__DOT__inst 
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
        bufp->chgIData(oldp+22,((0xfffff000U & vlSelf->ysyx_25040118_top__DOT__inst)),32);
        bufp->chgIData(oldp+23,((((- (IData)((vlSelf->ysyx_25040118_top__DOT__inst 
                                              >> 0x1fU))) 
                                  << 0x14U) | ((0xff000U 
                                                & vlSelf->ysyx_25040118_top__DOT__inst) 
                                               | ((0x800U 
                                                   & (vlSelf->ysyx_25040118_top__DOT__inst 
                                                      >> 9U)) 
                                                  | (0x7feU 
                                                     & (vlSelf->ysyx_25040118_top__DOT__inst 
                                                        >> 0x14U)))))),32);
        bufp->chgBit(oldp+24,(((0x63U != (0x7fU & vlSelf->ysyx_25040118_top__DOT__inst)) 
                               & ((0x23U != (0x7fU 
                                             & vlSelf->ysyx_25040118_top__DOT__inst)) 
                                  & (0U != (0x1fU & 
                                            (vlSelf->ysyx_25040118_top__DOT__inst 
                                             >> 7U)))))));
        bufp->chgIData(oldp+25,(vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file[0]),32);
        bufp->chgIData(oldp+26,(vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file[1]),32);
        bufp->chgIData(oldp+27,(vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file[2]),32);
        bufp->chgIData(oldp+28,(vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file[3]),32);
        bufp->chgIData(oldp+29,(vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file[4]),32);
        bufp->chgIData(oldp+30,(vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file[5]),32);
        bufp->chgIData(oldp+31,(vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file[6]),32);
        bufp->chgIData(oldp+32,(vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file[7]),32);
        bufp->chgIData(oldp+33,(vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file[8]),32);
        bufp->chgIData(oldp+34,(vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file[9]),32);
        bufp->chgIData(oldp+35,(vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file[10]),32);
        bufp->chgIData(oldp+36,(vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file[11]),32);
        bufp->chgIData(oldp+37,(vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file[12]),32);
        bufp->chgIData(oldp+38,(vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file[13]),32);
        bufp->chgIData(oldp+39,(vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file[14]),32);
        bufp->chgIData(oldp+40,(vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file[15]),32);
        bufp->chgIData(oldp+41,(vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file[16]),32);
        bufp->chgIData(oldp+42,(vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file[17]),32);
        bufp->chgIData(oldp+43,(vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file[18]),32);
        bufp->chgIData(oldp+44,(vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file[19]),32);
        bufp->chgIData(oldp+45,(vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file[20]),32);
        bufp->chgIData(oldp+46,(vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file[21]),32);
        bufp->chgIData(oldp+47,(vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file[22]),32);
        bufp->chgIData(oldp+48,(vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file[23]),32);
        bufp->chgIData(oldp+49,(vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file[24]),32);
        bufp->chgIData(oldp+50,(vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file[25]),32);
        bufp->chgIData(oldp+51,(vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file[26]),32);
        bufp->chgIData(oldp+52,(vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file[27]),32);
        bufp->chgIData(oldp+53,(vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file[28]),32);
        bufp->chgIData(oldp+54,(vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file[29]),32);
        bufp->chgIData(oldp+55,(vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file[30]),32);
        bufp->chgIData(oldp+56,(vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file[31]),32);
        bufp->chgIData(oldp+57,(vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__unnamedblk1__DOT__i),32);
    }
    bufp->chgBit(oldp+58,(vlSelf->clk));
    bufp->chgBit(oldp+59,(vlSelf->rst_n));
    bufp->chgBit(oldp+60,((1U & (~ (IData)(vlSelf->rst_n)))));
}

void Vysyx_25040118_top___024root__trace_cleanup(void* voidSelf, VerilatedFst* /*unused*/) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vysyx_25040118_top___024root__trace_cleanup\n"); );
    // Init
    Vysyx_25040118_top___024root* const __restrict vlSelf VL_ATTR_UNUSED = static_cast<Vysyx_25040118_top___024root*>(voidSelf);
    Vysyx_25040118_top__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    // Body
    vlSymsp->__Vm_activity = false;
    vlSymsp->TOP.__Vm_traceActivity[0U] = 0U;
    vlSymsp->TOP.__Vm_traceActivity[1U] = 0U;
}
