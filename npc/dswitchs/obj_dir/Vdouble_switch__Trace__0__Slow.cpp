// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Tracing implementation internals
#include "verilated_fst_c.h"
#include "Vdouble_switch__Syms.h"


VL_ATTR_COLD void Vdouble_switch___024root__trace_init_sub__TOP__0(Vdouble_switch___024root* vlSelf, VerilatedFst* tracep) {
    if (false && vlSelf) {}  // Prevent unused
    Vdouble_switch__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vdouble_switch___024root__trace_init_sub__TOP__0\n"); );
    // Init
    const int c = vlSymsp->__Vm_baseCode;
    // Body
    tracep->declBit(c+1,"a",-1,FST_VD_INPUT,FST_VT_VCD_WIRE, false,-1);
    tracep->declBit(c+2,"b",-1,FST_VD_INPUT,FST_VT_VCD_WIRE, false,-1);
    tracep->declBit(c+3,"f",-1,FST_VD_OUTPUT,FST_VT_VCD_WIRE, false,-1);
    tracep->pushNamePrefix("double_switch ");
    tracep->declBit(c+1,"a",-1,FST_VD_INPUT,FST_VT_VCD_WIRE, false,-1);
    tracep->declBit(c+2,"b",-1,FST_VD_INPUT,FST_VT_VCD_WIRE, false,-1);
    tracep->declBit(c+3,"f",-1,FST_VD_OUTPUT,FST_VT_VCD_WIRE, false,-1);
    tracep->popNamePrefix(1);
}

VL_ATTR_COLD void Vdouble_switch___024root__trace_init_top(Vdouble_switch___024root* vlSelf, VerilatedFst* tracep) {
    if (false && vlSelf) {}  // Prevent unused
    Vdouble_switch__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vdouble_switch___024root__trace_init_top\n"); );
    // Body
    Vdouble_switch___024root__trace_init_sub__TOP__0(vlSelf, tracep);
}

VL_ATTR_COLD void Vdouble_switch___024root__trace_full_top_0(void* voidSelf, VerilatedFst::Buffer* bufp);
void Vdouble_switch___024root__trace_chg_top_0(void* voidSelf, VerilatedFst::Buffer* bufp);
void Vdouble_switch___024root__trace_cleanup(void* voidSelf, VerilatedFst* /*unused*/);

VL_ATTR_COLD void Vdouble_switch___024root__trace_register(Vdouble_switch___024root* vlSelf, VerilatedFst* tracep) {
    if (false && vlSelf) {}  // Prevent unused
    Vdouble_switch__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vdouble_switch___024root__trace_register\n"); );
    // Body
    tracep->addFullCb(&Vdouble_switch___024root__trace_full_top_0, vlSelf);
    tracep->addChgCb(&Vdouble_switch___024root__trace_chg_top_0, vlSelf);
    tracep->addCleanupCb(&Vdouble_switch___024root__trace_cleanup, vlSelf);
}

VL_ATTR_COLD void Vdouble_switch___024root__trace_full_sub_0(Vdouble_switch___024root* vlSelf, VerilatedFst::Buffer* bufp);

VL_ATTR_COLD void Vdouble_switch___024root__trace_full_top_0(void* voidSelf, VerilatedFst::Buffer* bufp) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vdouble_switch___024root__trace_full_top_0\n"); );
    // Init
    Vdouble_switch___024root* const __restrict vlSelf VL_ATTR_UNUSED = static_cast<Vdouble_switch___024root*>(voidSelf);
    Vdouble_switch__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    // Body
    Vdouble_switch___024root__trace_full_sub_0((&vlSymsp->TOP), bufp);
}

VL_ATTR_COLD void Vdouble_switch___024root__trace_full_sub_0(Vdouble_switch___024root* vlSelf, VerilatedFst::Buffer* bufp) {
    if (false && vlSelf) {}  // Prevent unused
    Vdouble_switch__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vdouble_switch___024root__trace_full_sub_0\n"); );
    // Init
    uint32_t* const oldp VL_ATTR_UNUSED = bufp->oldp(vlSymsp->__Vm_baseCode);
    // Body
    bufp->fullBit(oldp+1,(vlSelf->a));
    bufp->fullBit(oldp+2,(vlSelf->b));
    bufp->fullBit(oldp+3,(vlSelf->f));
}
