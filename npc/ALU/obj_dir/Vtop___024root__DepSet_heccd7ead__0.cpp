// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vtop.h for the primary calling header

#include "verilated.h"

#include "Vtop___024root.h"

void Vtop___024root___eval_act(Vtop___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vtop__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtop___024root___eval_act\n"); );
}

VL_INLINE_OPT void Vtop___024root___nba_sequent__TOP__0(Vtop___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vtop__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtop___024root___nba_sequent__TOP__0\n"); );
    // Body
    if ((4U & (IData)(vlSelf->choose))) {
        if ((2U & (IData)(vlSelf->choose))) {
            if ((1U & (IData)(vlSelf->choose))) {
                vlSelf->cin = 0U;
                if (((IData)(vlSelf->a) == (IData)(vlSelf->b))) {
                    vlSelf->out = 1U;
                    vlSelf->m = 0U;
                } else {
                    vlSelf->out = 0U;
                    vlSelf->m = 0U;
                }
            } else if (((1U & ((IData)(vlSelf->a) >> 3U)) 
                        == (1U & ((IData)(vlSelf->b) 
                                  >> 3U)))) {
                vlSelf->cin = 0U;
                if (((IData)(vlSelf->a) < (IData)(vlSelf->b))) {
                    vlSelf->out = 1U;
                    vlSelf->m = 0U;
                } else {
                    vlSelf->out = 0U;
                    vlSelf->m = 0U;
                }
            } else {
                if ((IData)(((~ ((IData)(vlSelf->a) 
                                 >> 3U)) & ((IData)(vlSelf->b) 
                                            >> 3U)))) {
                    vlSelf->cin = 0U;
                    vlSelf->out = 0U;
                    vlSelf->m = 0U;
                }
                if ((IData)((((IData)(vlSelf->a) >> 3U) 
                             & (~ ((IData)(vlSelf->b) 
                                   >> 3U))))) {
                    vlSelf->cin = 0U;
                    vlSelf->out = 1U;
                    vlSelf->m = 0U;
                }
            }
        } else if ((1U & (IData)(vlSelf->choose))) {
            vlSelf->cin = (1U & (((IData)(vlSelf->a) 
                                  ^ (IData)(vlSelf->b)) 
                                 >> 4U));
            vlSelf->out = ((IData)(vlSelf->a) ^ (IData)(vlSelf->b));
            vlSelf->m = 0U;
        } else {
            vlSelf->cin = (1U & (((IData)(vlSelf->a) 
                                  | (IData)(vlSelf->b)) 
                                 >> 4U));
            vlSelf->out = ((IData)(vlSelf->a) | (IData)(vlSelf->b));
            vlSelf->m = 0U;
        }
    } else if ((2U & (IData)(vlSelf->choose))) {
        vlSelf->cin = 0U;
        if ((1U & (IData)(vlSelf->choose))) {
            vlSelf->out = ((IData)(vlSelf->a) & (IData)(vlSelf->b));
            vlSelf->m = 0U;
        } else {
            vlSelf->out = (0xfU & (~ (IData)(vlSelf->a)));
            vlSelf->m = 0U;
        }
    } else if ((1U & (IData)(vlSelf->choose))) {
        vlSelf->cin = (1U & (1U & ((VL_EXTENDS_II(5,4, (IData)(vlSelf->a)) 
                                    - VL_EXTENDS_II(5,4, (IData)(vlSelf->b))) 
                                   >> 4U)));
        vlSelf->out = (0xfU & ((0x1fU & VL_EXTENDS_II(5,4, (IData)(vlSelf->a))) 
                               - (0x1fU & VL_EXTENDS_II(5,4, (IData)(vlSelf->b)))));
        vlSelf->m = (1U & (((((IData)(vlSelf->a) & (IData)(vlSelf->b)) 
                             >> 3U) & (~ ((IData)(vlSelf->out) 
                                          >> 3U))) 
                           | (((~ ((IData)(vlSelf->a) 
                                   >> 3U)) & (~ ((IData)(vlSelf->b) 
                                                 >> 3U))) 
                              & ((IData)(vlSelf->out) 
                                 >> 3U))));
    } else {
        vlSelf->cin = (1U & (1U & (((IData)(vlSelf->a) 
                                    + (IData)(vlSelf->b)) 
                                   >> 4U)));
        vlSelf->out = (0xfU & ((IData)(vlSelf->a) + (IData)(vlSelf->b)));
        vlSelf->m = (1U & (((((IData)(vlSelf->a) & (IData)(vlSelf->b)) 
                             >> 3U) & (~ ((IData)(vlSelf->out) 
                                          >> 3U))) 
                           | (((~ ((IData)(vlSelf->a) 
                                   >> 3U)) & (~ ((IData)(vlSelf->b) 
                                                 >> 3U))) 
                              & ((IData)(vlSelf->out) 
                                 >> 3U))));
    }
}

void Vtop___024root___eval_nba(Vtop___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vtop__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtop___024root___eval_nba\n"); );
    // Body
    if (vlSelf->__VnbaTriggered.at(0U)) {
        Vtop___024root___nba_sequent__TOP__0(vlSelf);
    }
}

void Vtop___024root___eval_triggers__act(Vtop___024root* vlSelf);
#ifdef VL_DEBUG
VL_ATTR_COLD void Vtop___024root___dump_triggers__act(Vtop___024root* vlSelf);
#endif  // VL_DEBUG
#ifdef VL_DEBUG
VL_ATTR_COLD void Vtop___024root___dump_triggers__nba(Vtop___024root* vlSelf);
#endif  // VL_DEBUG

void Vtop___024root___eval(Vtop___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vtop__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtop___024root___eval\n"); );
    // Init
    VlTriggerVec<1> __VpreTriggered;
    IData/*31:0*/ __VnbaIterCount;
    CData/*0:0*/ __VnbaContinue;
    // Body
    __VnbaIterCount = 0U;
    __VnbaContinue = 1U;
    while (__VnbaContinue) {
        __VnbaContinue = 0U;
        vlSelf->__VnbaTriggered.clear();
        vlSelf->__VactIterCount = 0U;
        vlSelf->__VactContinue = 1U;
        while (vlSelf->__VactContinue) {
            vlSelf->__VactContinue = 0U;
            Vtop___024root___eval_triggers__act(vlSelf);
            if (vlSelf->__VactTriggered.any()) {
                vlSelf->__VactContinue = 1U;
                if (VL_UNLIKELY((0x64U < vlSelf->__VactIterCount))) {
#ifdef VL_DEBUG
                    Vtop___024root___dump_triggers__act(vlSelf);
#endif
                    VL_FATAL_MT("/home/pz40/ysyx-workbench/npc/ALU/vsrc/top.v", 1, "", "Active region did not converge.");
                }
                vlSelf->__VactIterCount = ((IData)(1U) 
                                           + vlSelf->__VactIterCount);
                __VpreTriggered.andNot(vlSelf->__VactTriggered, vlSelf->__VnbaTriggered);
                vlSelf->__VnbaTriggered.set(vlSelf->__VactTriggered);
                Vtop___024root___eval_act(vlSelf);
            }
        }
        if (vlSelf->__VnbaTriggered.any()) {
            __VnbaContinue = 1U;
            if (VL_UNLIKELY((0x64U < __VnbaIterCount))) {
#ifdef VL_DEBUG
                Vtop___024root___dump_triggers__nba(vlSelf);
#endif
                VL_FATAL_MT("/home/pz40/ysyx-workbench/npc/ALU/vsrc/top.v", 1, "", "NBA region did not converge.");
            }
            __VnbaIterCount = ((IData)(1U) + __VnbaIterCount);
            Vtop___024root___eval_nba(vlSelf);
        }
    }
}

#ifdef VL_DEBUG
void Vtop___024root___eval_debug_assertions(Vtop___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vtop__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtop___024root___eval_debug_assertions\n"); );
    // Body
    if (VL_UNLIKELY((vlSelf->clk & 0xfeU))) {
        Verilated::overWidthError("clk");}
    if (VL_UNLIKELY((vlSelf->choose & 0xf8U))) {
        Verilated::overWidthError("choose");}
    if (VL_UNLIKELY((vlSelf->a & 0xf0U))) {
        Verilated::overWidthError("a");}
    if (VL_UNLIKELY((vlSelf->b & 0xf0U))) {
        Verilated::overWidthError("b");}
}
#endif  // VL_DEBUG
