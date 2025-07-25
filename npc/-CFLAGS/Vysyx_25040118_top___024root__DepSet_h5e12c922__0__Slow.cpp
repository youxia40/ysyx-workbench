// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vysyx_25040118_top.h for the primary calling header

#include "verilated.h"
#include "verilated_dpi.h"

#include "Vysyx_25040118_top___024root.h"

VL_ATTR_COLD void Vysyx_25040118_top___024root___eval_static(Vysyx_25040118_top___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vysyx_25040118_top__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vysyx_25040118_top___024root___eval_static\n"); );
}

VL_ATTR_COLD void Vysyx_25040118_top___024root___eval_initial(Vysyx_25040118_top___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vysyx_25040118_top__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vysyx_25040118_top___024root___eval_initial\n"); );
    // Body
    vlSelf->__Vtrigrprev__TOP__clk = vlSelf->clk;
}

VL_ATTR_COLD void Vysyx_25040118_top___024root___eval_final(Vysyx_25040118_top___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vysyx_25040118_top__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vysyx_25040118_top___024root___eval_final\n"); );
}

VL_ATTR_COLD void Vysyx_25040118_top___024root___eval_triggers__stl(Vysyx_25040118_top___024root* vlSelf);
#ifdef VL_DEBUG
VL_ATTR_COLD void Vysyx_25040118_top___024root___dump_triggers__stl(Vysyx_25040118_top___024root* vlSelf);
#endif  // VL_DEBUG
VL_ATTR_COLD void Vysyx_25040118_top___024root___eval_stl(Vysyx_25040118_top___024root* vlSelf);

VL_ATTR_COLD void Vysyx_25040118_top___024root___eval_settle(Vysyx_25040118_top___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vysyx_25040118_top__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vysyx_25040118_top___024root___eval_settle\n"); );
    // Init
    CData/*0:0*/ __VstlContinue;
    // Body
    vlSelf->__VstlIterCount = 0U;
    __VstlContinue = 1U;
    while (__VstlContinue) {
        __VstlContinue = 0U;
        Vysyx_25040118_top___024root___eval_triggers__stl(vlSelf);
        if (vlSelf->__VstlTriggered.any()) {
            __VstlContinue = 1U;
            if (VL_UNLIKELY((0x64U < vlSelf->__VstlIterCount))) {
#ifdef VL_DEBUG
                Vysyx_25040118_top___024root___dump_triggers__stl(vlSelf);
#endif
                VL_FATAL_MT("/home/pz40/ysyx-workbench/npc/vsrc/ysyx_25040118_top.v", 1, "", "Settle region did not converge.");
            }
            vlSelf->__VstlIterCount = ((IData)(1U) 
                                       + vlSelf->__VstlIterCount);
            Vysyx_25040118_top___024root___eval_stl(vlSelf);
        }
    }
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vysyx_25040118_top___024root___dump_triggers__stl(Vysyx_25040118_top___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vysyx_25040118_top__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vysyx_25040118_top___024root___dump_triggers__stl\n"); );
    // Body
    if ((1U & (~ (IData)(vlSelf->__VstlTriggered.any())))) {
        VL_DBG_MSGF("         No triggers active\n");
    }
    if (vlSelf->__VstlTriggered.at(0U)) {
        VL_DBG_MSGF("         'stl' region trigger index 0 is active: Internal 'stl' trigger - first iteration\n");
    }
}
#endif  // VL_DEBUG

void Vysyx_25040118_top___024root____Vdpiimwrap_ysyx_25040118_top__DOT__ifu__DOT__pmem_read_TOP(IData/*31:0*/ raddr, IData/*31:0*/ &pmem_read__Vfuncrtn);

VL_ATTR_COLD void Vysyx_25040118_top___024root___stl_sequent__TOP__0(Vysyx_25040118_top___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vysyx_25040118_top__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vysyx_25040118_top___024root___stl_sequent__TOP__0\n"); );
    // Body
    vlSelf->ysyx_25040118_top__DOT__exu__DOT____VdfgTmp_h8a6dd40b__0 
        = (vlSelf->ysyx_25040118_top__DOT__imm + vlSelf->ysyx_25040118_top__DOT__pc);
    Vysyx_25040118_top___024root____Vdpiimwrap_ysyx_25040118_top__DOT__ifu__DOT__pmem_read_TOP(vlSelf->ysyx_25040118_top__DOT__pc, vlSelf->__Vfunc_ysyx_25040118_top__DOT__ifu__DOT__pmem_read__0__Vfuncout);
    vlSelf->ysyx_25040118_top__DOT__inst = vlSelf->__Vfunc_ysyx_25040118_top__DOT__ifu__DOT__pmem_read__0__Vfuncout;
    vlSelf->ysyx_25040118_top__DOT__mem_wmask = ((0U 
                                                  == 
                                                  (7U 
                                                   & (vlSelf->ysyx_25040118_top__DOT__inst 
                                                      >> 0xcU)))
                                                  ? 1U
                                                  : 
                                                 ((1U 
                                                   == 
                                                   (7U 
                                                    & (vlSelf->ysyx_25040118_top__DOT__inst 
                                                       >> 0xcU)))
                                                   ? 3U
                                                   : 0xfU));
    vlSelf->ysyx_25040118_top__DOT__alu_ctrl = ((0x40U 
                                                 & vlSelf->ysyx_25040118_top__DOT__inst)
                                                 ? 
                                                ((0x20U 
                                                  & vlSelf->ysyx_25040118_top__DOT__inst)
                                                  ? 
                                                 ((0x10U 
                                                   & vlSelf->ysyx_25040118_top__DOT__inst)
                                                   ? 0U
                                                   : 
                                                  ((8U 
                                                    & vlSelf->ysyx_25040118_top__DOT__inst)
                                                    ? 
                                                   ((4U 
                                                     & vlSelf->ysyx_25040118_top__DOT__inst)
                                                     ? 
                                                    ((2U 
                                                      & vlSelf->ysyx_25040118_top__DOT__inst)
                                                      ? 
                                                     ((1U 
                                                       & vlSelf->ysyx_25040118_top__DOT__inst)
                                                       ? 1U
                                                       : 0U)
                                                      : 0U)
                                                     : 0U)
                                                    : 
                                                   ((4U 
                                                     & vlSelf->ysyx_25040118_top__DOT__inst)
                                                     ? 
                                                    ((2U 
                                                      & vlSelf->ysyx_25040118_top__DOT__inst)
                                                      ? 
                                                     ((1U 
                                                       & vlSelf->ysyx_25040118_top__DOT__inst)
                                                       ? 1U
                                                       : 0U)
                                                      : 0U)
                                                     : 
                                                    ((2U 
                                                      & vlSelf->ysyx_25040118_top__DOT__inst)
                                                      ? 
                                                     ((1U 
                                                       & vlSelf->ysyx_25040118_top__DOT__inst)
                                                       ? 
                                                      ((0x4000U 
                                                        & vlSelf->ysyx_25040118_top__DOT__inst)
                                                        ? 
                                                       ((0x2000U 
                                                         & vlSelf->ysyx_25040118_top__DOT__inst)
                                                         ? 0U
                                                         : 2U)
                                                        : 2U)
                                                       : 0U)
                                                      : 0U))))
                                                  : 0U)
                                                 : 
                                                ((0x20U 
                                                  & vlSelf->ysyx_25040118_top__DOT__inst)
                                                  ? 
                                                 ((0x10U 
                                                   & vlSelf->ysyx_25040118_top__DOT__inst)
                                                   ? 0U
                                                   : 
                                                  ((8U 
                                                    & vlSelf->ysyx_25040118_top__DOT__inst)
                                                    ? 0U
                                                    : 
                                                   ((4U 
                                                     & vlSelf->ysyx_25040118_top__DOT__inst)
                                                     ? 0U
                                                     : 
                                                    ((2U 
                                                      & vlSelf->ysyx_25040118_top__DOT__inst)
                                                      ? 
                                                     ((1U 
                                                       & vlSelf->ysyx_25040118_top__DOT__inst)
                                                       ? 1U
                                                       : 0U)
                                                      : 0U))))
                                                  : 
                                                 ((0x10U 
                                                   & vlSelf->ysyx_25040118_top__DOT__inst)
                                                   ? 
                                                  ((8U 
                                                    & vlSelf->ysyx_25040118_top__DOT__inst)
                                                    ? 0U
                                                    : 
                                                   ((4U 
                                                     & vlSelf->ysyx_25040118_top__DOT__inst)
                                                     ? 0U
                                                     : 
                                                    ((2U 
                                                      & vlSelf->ysyx_25040118_top__DOT__inst)
                                                      ? 
                                                     ((1U 
                                                       & vlSelf->ysyx_25040118_top__DOT__inst)
                                                       ? 
                                                      ((0x4000U 
                                                        & vlSelf->ysyx_25040118_top__DOT__inst)
                                                        ? 
                                                       ((0x2000U 
                                                         & vlSelf->ysyx_25040118_top__DOT__inst)
                                                         ? 
                                                        ((0x1000U 
                                                          & vlSelf->ysyx_25040118_top__DOT__inst)
                                                          ? 3U
                                                          : 4U)
                                                         : 
                                                        ((0x1000U 
                                                          & vlSelf->ysyx_25040118_top__DOT__inst)
                                                          ? 
                                                         ((0U 
                                                           == 
                                                           (vlSelf->ysyx_25040118_top__DOT__inst 
                                                            >> 0x19U))
                                                           ? 7U
                                                           : 8U)
                                                          : 5U))
                                                        : 
                                                       ((0x2000U 
                                                         & vlSelf->ysyx_25040118_top__DOT__inst)
                                                         ? 
                                                        ((0x1000U 
                                                          & vlSelf->ysyx_25040118_top__DOT__inst)
                                                          ? 0xaU
                                                          : 9U)
                                                         : 
                                                        ((0x1000U 
                                                          & vlSelf->ysyx_25040118_top__DOT__inst)
                                                          ? 6U
                                                          : 1U)))
                                                       : 0U)
                                                      : 0U)))
                                                   : 
                                                  ((8U 
                                                    & vlSelf->ysyx_25040118_top__DOT__inst)
                                                    ? 0U
                                                    : 
                                                   ((4U 
                                                     & vlSelf->ysyx_25040118_top__DOT__inst)
                                                     ? 0U
                                                     : 
                                                    ((2U 
                                                      & vlSelf->ysyx_25040118_top__DOT__inst)
                                                      ? 
                                                     ((1U 
                                                       & vlSelf->ysyx_25040118_top__DOT__inst)
                                                       ? 1U
                                                       : 0U)
                                                      : 0U))))));
    vlSelf->ysyx_25040118_top__DOT__reg_src2 = ((0U 
                                                 == 
                                                 (0x1fU 
                                                  & (vlSelf->ysyx_25040118_top__DOT__inst 
                                                     >> 0x14U)))
                                                 ? 0U
                                                 : 
                                                vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file
                                                [(0x1fU 
                                                  & (vlSelf->ysyx_25040118_top__DOT__inst 
                                                     >> 0x14U))]);
    vlSelf->ysyx_25040118_top__DOT__reg_src1 = ((0U 
                                                 == 
                                                 (0x1fU 
                                                  & (vlSelf->ysyx_25040118_top__DOT__inst 
                                                     >> 0xfU)))
                                                 ? 0U
                                                 : 
                                                vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file
                                                [(0x1fU 
                                                  & (vlSelf->ysyx_25040118_top__DOT__inst 
                                                     >> 0xfU))]);
    vlSelf->ysyx_25040118_top__DOT__exu__DOT__alu_result 
        = ((2U & (IData)(vlSelf->ysyx_25040118_top__DOT__alu_ctrl))
            ? ((1U & (IData)(vlSelf->ysyx_25040118_top__DOT__alu_ctrl))
                ? ((2U == (7U & (vlSelf->ysyx_25040118_top__DOT__inst 
                                 >> 0xcU))) ? (VL_LTS_III(32, vlSelf->ysyx_25040118_top__DOT__reg_src1, vlSelf->ysyx_25040118_top__DOT__reg_src2)
                                                ? 1U
                                                : 0U)
                    : ((3U == (7U & (vlSelf->ysyx_25040118_top__DOT__inst 
                                     >> 0xcU))) ? (
                                                   (vlSelf->ysyx_25040118_top__DOT__reg_src1 
                                                    < vlSelf->ysyx_25040118_top__DOT__reg_src2)
                                                    ? 1U
                                                    : 0U)
                        : 0U)) : ((0U == (7U & (vlSelf->ysyx_25040118_top__DOT__inst 
                                                >> 0xcU)))
                                   ? (vlSelf->ysyx_25040118_top__DOT__reg_src1 
                                      << (0x1fU & vlSelf->ysyx_25040118_top__DOT__reg_src2))
                                   : ((4U == (7U & 
                                              (vlSelf->ysyx_25040118_top__DOT__inst 
                                               >> 0xcU)))
                                       ? (vlSelf->ysyx_25040118_top__DOT__reg_src1 
                                          >> (0x1fU 
                                              & vlSelf->ysyx_25040118_top__DOT__reg_src2))
                                       : ((5U == (7U 
                                                  & (vlSelf->ysyx_25040118_top__DOT__inst 
                                                     >> 0xcU)))
                                           ? VL_SHIFTRS_III(32,32,5, vlSelf->ysyx_25040118_top__DOT__reg_src1, 
                                                            (0x1fU 
                                                             & vlSelf->ysyx_25040118_top__DOT__reg_src2))
                                           : 0U))))
            : ((1U & (IData)(vlSelf->ysyx_25040118_top__DOT__alu_ctrl))
                ? ((0U == (7U & (vlSelf->ysyx_25040118_top__DOT__inst 
                                 >> 0xcU))) ? (vlSelf->ysyx_25040118_top__DOT__reg_src1 
                                               & vlSelf->ysyx_25040118_top__DOT__reg_src2)
                    : ((7U == (7U & (vlSelf->ysyx_25040118_top__DOT__inst 
                                     >> 0xcU))) ? (vlSelf->ysyx_25040118_top__DOT__reg_src1 
                                                   | vlSelf->ysyx_25040118_top__DOT__reg_src2)
                        : ((6U == (7U & (vlSelf->ysyx_25040118_top__DOT__inst 
                                         >> 0xcU)))
                            ? (vlSelf->ysyx_25040118_top__DOT__reg_src1 
                               ^ vlSelf->ysyx_25040118_top__DOT__reg_src2)
                            : 0U))) : ((0U == (7U & 
                                               (vlSelf->ysyx_25040118_top__DOT__inst 
                                                >> 0xcU)))
                                        ? (vlSelf->ysyx_25040118_top__DOT__reg_src1 
                                           + vlSelf->ysyx_25040118_top__DOT__reg_src2)
                                        : ((7U == (7U 
                                                   & (vlSelf->ysyx_25040118_top__DOT__inst 
                                                      >> 0xcU)))
                                            ? (vlSelf->ysyx_25040118_top__DOT__reg_src1 
                                               - vlSelf->ysyx_25040118_top__DOT__reg_src2)
                                            : 0U))));
    vlSelf->ysyx_25040118_top__DOT__exu__DOT____VdfgTmp_h623bd169__0 
        = (vlSelf->ysyx_25040118_top__DOT__imm + vlSelf->ysyx_25040118_top__DOT__reg_src1);
    vlSelf->ysyx_25040118_top__DOT__mem_addr = (((3U 
                                                  == 
                                                  (0x7fU 
                                                   & vlSelf->ysyx_25040118_top__DOT__inst)) 
                                                 | (0x23U 
                                                    == 
                                                    (0x7fU 
                                                     & vlSelf->ysyx_25040118_top__DOT__inst)))
                                                 ? vlSelf->ysyx_25040118_top__DOT__exu__DOT____VdfgTmp_h623bd169__0
                                                 : 0U);
    vlSelf->ysyx_25040118_top__DOT__next_pc = ((0x6fU 
                                                == 
                                                (0x7fU 
                                                 & vlSelf->ysyx_25040118_top__DOT__inst))
                                                ? vlSelf->ysyx_25040118_top__DOT__exu__DOT____VdfgTmp_h8a6dd40b__0
                                                : (
                                                   (0x67U 
                                                    == 
                                                    (0x7fU 
                                                     & vlSelf->ysyx_25040118_top__DOT__inst))
                                                    ? 
                                                   (0xfffffffeU 
                                                    & vlSelf->ysyx_25040118_top__DOT__exu__DOT____VdfgTmp_h623bd169__0)
                                                    : 
                                                   (((0x63U 
                                                      == 
                                                      (0x7fU 
                                                       & vlSelf->ysyx_25040118_top__DOT__inst))
                                                      ? 
                                                     ((0U 
                                                       == 
                                                       (7U 
                                                        & (vlSelf->ysyx_25040118_top__DOT__inst 
                                                           >> 0xcU)))
                                                       ? 
                                                      (vlSelf->ysyx_25040118_top__DOT__reg_src1 
                                                       == vlSelf->ysyx_25040118_top__DOT__reg_src2)
                                                       : (IData)(
                                                                 ((0x1000U 
                                                                   == 
                                                                   (0x7000U 
                                                                    & vlSelf->ysyx_25040118_top__DOT__inst)) 
                                                                  & (vlSelf->ysyx_25040118_top__DOT__reg_src1 
                                                                     != vlSelf->ysyx_25040118_top__DOT__reg_src2))))
                                                      : 
                                                     ((0x63U 
                                                       == 
                                                       (0x7fU 
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
                                                     : 
                                                    ((IData)(4U) 
                                                     + vlSelf->ysyx_25040118_top__DOT__pc))));
}

VL_ATTR_COLD void Vysyx_25040118_top___024root___eval_stl(Vysyx_25040118_top___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vysyx_25040118_top__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vysyx_25040118_top___024root___eval_stl\n"); );
    // Body
    if (vlSelf->__VstlTriggered.at(0U)) {
        Vysyx_25040118_top___024root___stl_sequent__TOP__0(vlSelf);
        vlSelf->__Vm_traceActivity[1U] = 1U;
        vlSelf->__Vm_traceActivity[0U] = 1U;
    }
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vysyx_25040118_top___024root___dump_triggers__act(Vysyx_25040118_top___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vysyx_25040118_top__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vysyx_25040118_top___024root___dump_triggers__act\n"); );
    // Body
    if ((1U & (~ (IData)(vlSelf->__VactTriggered.any())))) {
        VL_DBG_MSGF("         No triggers active\n");
    }
    if (vlSelf->__VactTriggered.at(0U)) {
        VL_DBG_MSGF("         'act' region trigger index 0 is active: @(posedge clk)\n");
    }
}
#endif  // VL_DEBUG

#ifdef VL_DEBUG
VL_ATTR_COLD void Vysyx_25040118_top___024root___dump_triggers__nba(Vysyx_25040118_top___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vysyx_25040118_top__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vysyx_25040118_top___024root___dump_triggers__nba\n"); );
    // Body
    if ((1U & (~ (IData)(vlSelf->__VnbaTriggered.any())))) {
        VL_DBG_MSGF("         No triggers active\n");
    }
    if (vlSelf->__VnbaTriggered.at(0U)) {
        VL_DBG_MSGF("         'nba' region trigger index 0 is active: @(posedge clk)\n");
    }
}
#endif  // VL_DEBUG

VL_ATTR_COLD void Vysyx_25040118_top___024root___ctor_var_reset(Vysyx_25040118_top___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vysyx_25040118_top__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vysyx_25040118_top___024root___ctor_var_reset\n"); );
    // Body
    vlSelf->clk = VL_RAND_RESET_I(1);
    vlSelf->rst_n = VL_RAND_RESET_I(1);
    vlSelf->ysyx_25040118_top__DOT__pc = VL_RAND_RESET_I(32);
    vlSelf->ysyx_25040118_top__DOT__next_pc = VL_RAND_RESET_I(32);
    vlSelf->ysyx_25040118_top__DOT__inst = VL_RAND_RESET_I(32);
    vlSelf->ysyx_25040118_top__DOT__imm = VL_RAND_RESET_I(32);
    vlSelf->ysyx_25040118_top__DOT__alu_ctrl = VL_RAND_RESET_I(5);
    vlSelf->ysyx_25040118_top__DOT__exu_result = VL_RAND_RESET_I(32);
    vlSelf->ysyx_25040118_top__DOT__reg_src1 = VL_RAND_RESET_I(32);
    vlSelf->ysyx_25040118_top__DOT__reg_src2 = VL_RAND_RESET_I(32);
    vlSelf->ysyx_25040118_top__DOT__mem_addr = VL_RAND_RESET_I(32);
    vlSelf->ysyx_25040118_top__DOT__mem_wmask = VL_RAND_RESET_I(4);
    for (int __Vi0 = 0; __Vi0 < 32; ++__Vi0) {
        vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file[__Vi0] = VL_RAND_RESET_I(32);
    }
    vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__unnamedblk1__DOT__i = 0;
    vlSelf->ysyx_25040118_top__DOT__exu__DOT__branch_taken = VL_RAND_RESET_I(1);
    vlSelf->ysyx_25040118_top__DOT__exu__DOT__alu_result = VL_RAND_RESET_I(32);
    vlSelf->ysyx_25040118_top__DOT__exu__DOT____VdfgTmp_h8a6dd40b__0 = 0;
    vlSelf->ysyx_25040118_top__DOT__exu__DOT____VdfgTmp_h623bd169__0 = 0;
    for (int __Vi0 = 0; __Vi0 < 1024; ++__Vi0) {
        vlSelf->ysyx_25040118_top__DOT__mem__DOT__dmem[__Vi0] = VL_RAND_RESET_I(32);
    }
    vlSelf->__Vfunc_ysyx_25040118_top__DOT__ifu__DOT__pmem_read__0__Vfuncout = 0;
    vlSelf->__Vtrigrprev__TOP__clk = VL_RAND_RESET_I(1);
    for (int __Vi0 = 0; __Vi0 < 2; ++__Vi0) {
        vlSelf->__Vm_traceActivity[__Vi0] = 0;
    }
}
