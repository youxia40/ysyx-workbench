// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vysyx_25040118_top.h for the primary calling header

#include "verilated.h"
#include "verilated_dpi.h"

#include "Vysyx_25040118_top___024root.h"

void Vysyx_25040118_top___024root___eval_act(Vysyx_25040118_top___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vysyx_25040118_top__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vysyx_25040118_top___024root___eval_act\n"); );
}

void Vysyx_25040118_top___024root____Vdpiimwrap_ysyx_25040118_top__DOT__ifu__DOT__pmem_read_TOP(IData/*31:0*/ raddr, IData/*31:0*/ &pmem_read__Vfuncrtn);

VL_INLINE_OPT void Vysyx_25040118_top___024root___nba_sequent__TOP__0(Vysyx_25040118_top___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vysyx_25040118_top__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vysyx_25040118_top___024root___nba_sequent__TOP__0\n"); );
    // Init
    CData/*4:0*/ __Vdlyvdim0__ysyx_25040118_top__DOT__reg_file__DOT__reg_file__v0;
    __Vdlyvdim0__ysyx_25040118_top__DOT__reg_file__DOT__reg_file__v0 = 0;
    IData/*31:0*/ __Vdlyvval__ysyx_25040118_top__DOT__reg_file__DOT__reg_file__v0;
    __Vdlyvval__ysyx_25040118_top__DOT__reg_file__DOT__reg_file__v0 = 0;
    CData/*0:0*/ __Vdlyvset__ysyx_25040118_top__DOT__reg_file__DOT__reg_file__v0;
    __Vdlyvset__ysyx_25040118_top__DOT__reg_file__DOT__reg_file__v0 = 0;
    CData/*0:0*/ __Vdlyvset__ysyx_25040118_top__DOT__reg_file__DOT__reg_file__v1;
    __Vdlyvset__ysyx_25040118_top__DOT__reg_file__DOT__reg_file__v1 = 0;
    SData/*9:0*/ __Vdlyvdim0__ysyx_25040118_top__DOT__mem__DOT__dmem__v0;
    __Vdlyvdim0__ysyx_25040118_top__DOT__mem__DOT__dmem__v0 = 0;
    CData/*4:0*/ __Vdlyvlsb__ysyx_25040118_top__DOT__mem__DOT__dmem__v0;
    __Vdlyvlsb__ysyx_25040118_top__DOT__mem__DOT__dmem__v0 = 0;
    CData/*7:0*/ __Vdlyvval__ysyx_25040118_top__DOT__mem__DOT__dmem__v0;
    __Vdlyvval__ysyx_25040118_top__DOT__mem__DOT__dmem__v0 = 0;
    CData/*0:0*/ __Vdlyvset__ysyx_25040118_top__DOT__mem__DOT__dmem__v0;
    __Vdlyvset__ysyx_25040118_top__DOT__mem__DOT__dmem__v0 = 0;
    SData/*9:0*/ __Vdlyvdim0__ysyx_25040118_top__DOT__mem__DOT__dmem__v1;
    __Vdlyvdim0__ysyx_25040118_top__DOT__mem__DOT__dmem__v1 = 0;
    CData/*4:0*/ __Vdlyvlsb__ysyx_25040118_top__DOT__mem__DOT__dmem__v1;
    __Vdlyvlsb__ysyx_25040118_top__DOT__mem__DOT__dmem__v1 = 0;
    SData/*15:0*/ __Vdlyvval__ysyx_25040118_top__DOT__mem__DOT__dmem__v1;
    __Vdlyvval__ysyx_25040118_top__DOT__mem__DOT__dmem__v1 = 0;
    CData/*0:0*/ __Vdlyvset__ysyx_25040118_top__DOT__mem__DOT__dmem__v1;
    __Vdlyvset__ysyx_25040118_top__DOT__mem__DOT__dmem__v1 = 0;
    SData/*9:0*/ __Vdlyvdim0__ysyx_25040118_top__DOT__mem__DOT__dmem__v2;
    __Vdlyvdim0__ysyx_25040118_top__DOT__mem__DOT__dmem__v2 = 0;
    IData/*31:0*/ __Vdlyvval__ysyx_25040118_top__DOT__mem__DOT__dmem__v2;
    __Vdlyvval__ysyx_25040118_top__DOT__mem__DOT__dmem__v2 = 0;
    CData/*0:0*/ __Vdlyvset__ysyx_25040118_top__DOT__mem__DOT__dmem__v2;
    __Vdlyvset__ysyx_25040118_top__DOT__mem__DOT__dmem__v2 = 0;
    // Body
    if (VL_LIKELY(vlSelf->rst_n)) {
        if (VL_UNLIKELY((((0x63U != (0x7fU & vlSelf->ysyx_25040118_top__DOT__inst)) 
                          & ((0x23U != (0x7fU & vlSelf->ysyx_25040118_top__DOT__inst)) 
                             & (0U != (0x1fU & (vlSelf->ysyx_25040118_top__DOT__inst 
                                                >> 7U))))) 
                         & (0U != (0x1fU & (vlSelf->ysyx_25040118_top__DOT__inst 
                                            >> 7U)))))) {
            VL_WRITEF("[REG] Write: addr=x%02#, data=0x%08x\n",
                      5,(0x1fU & (vlSelf->ysyx_25040118_top__DOT__inst 
                                  >> 7U)),32,vlSelf->ysyx_25040118_top__DOT__exu_result);
        }
    } else {
        VL_WRITEF("[REG] Reset...\n");
    }
    if (VL_LIKELY(vlSelf->rst_n)) {
        if (VL_UNLIKELY((0x23U == (0x7fU & vlSelf->ysyx_25040118_top__DOT__inst)))) {
            VL_WRITEF("[MEM] Write: addr=0x%08x, data=0x%08x, mask=%04b\n",
                      32,vlSelf->ysyx_25040118_top__DOT__mem_addr,
                      32,vlSelf->ysyx_25040118_top__DOT__reg_src2,
                      4,(IData)(vlSelf->ysyx_25040118_top__DOT__mem_wmask));
        }
    } else {
        VL_WRITEF("[MEM] Reset...\n");
    }
    __Vdlyvset__ysyx_25040118_top__DOT__mem__DOT__dmem__v0 = 0U;
    __Vdlyvset__ysyx_25040118_top__DOT__mem__DOT__dmem__v1 = 0U;
    __Vdlyvset__ysyx_25040118_top__DOT__mem__DOT__dmem__v2 = 0U;
    __Vdlyvset__ysyx_25040118_top__DOT__reg_file__DOT__reg_file__v0 = 0U;
    __Vdlyvset__ysyx_25040118_top__DOT__reg_file__DOT__reg_file__v1 = 0U;
    if (VL_UNLIKELY((0x100073U == vlSelf->ysyx_25040118_top__DOT__inst))) {
        VL_WRITEF("[TOP] EBREAK encountered at pc=0x%08x\n",
                  32,vlSelf->ysyx_25040118_top__DOT__pc);
        VL_FINISH_MT("/home/pz40/ysyx-workbench/npc/vsrc/ysyx_25040118_top.v", 97, "");
    }
    if (vlSelf->rst_n) {
        VL_WRITEF("[IFU] PC=0x%08x, inst=0x%08x\n",
                  32,vlSelf->ysyx_25040118_top__DOT__pc,
                  32,vlSelf->ysyx_25040118_top__DOT__inst);
    } else {
        VL_WRITEF("[IFU] Reset...\n");
    }
    if (VL_LIKELY(vlSelf->rst_n)) {
        if (VL_UNLIKELY(((0x63U == (0x7fU & vlSelf->ysyx_25040118_top__DOT__inst))
                          ? ((0U == (7U & (vlSelf->ysyx_25040118_top__DOT__inst 
                                           >> 0xcU)))
                              ? (vlSelf->ysyx_25040118_top__DOT__reg_src1 
                                 == vlSelf->ysyx_25040118_top__DOT__reg_src2)
                              : (IData)(((0x1000U == 
                                          (0x7000U 
                                           & vlSelf->ysyx_25040118_top__DOT__inst)) 
                                         & (vlSelf->ysyx_25040118_top__DOT__reg_src1 
                                            != vlSelf->ysyx_25040118_top__DOT__reg_src2))))
                          : ((0x63U == (0x7fU & vlSelf->ysyx_25040118_top__DOT__inst)) 
                             & ((4U == (7U & (vlSelf->ysyx_25040118_top__DOT__inst 
                                              >> 0xcU)))
                                 ? VL_LTS_III(32, vlSelf->ysyx_25040118_top__DOT__reg_src1, vlSelf->ysyx_25040118_top__DOT__reg_src2)
                                 : ((5U == (7U & (vlSelf->ysyx_25040118_top__DOT__inst 
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
                                                       >= vlSelf->ysyx_25040118_top__DOT__reg_src2)))))))))) {
            VL_WRITEF("[EXU] Branch taken at pc=0x%08x, target=0x%08x\n",
                      32,vlSelf->ysyx_25040118_top__DOT__pc,
                      32,((0x6fU == (0x7fU & vlSelf->ysyx_25040118_top__DOT__inst))
                           ? vlSelf->ysyx_25040118_top__DOT__exu__DOT____VdfgTmp_h8a6dd40b__0
                           : ((0x67U == (0x7fU & vlSelf->ysyx_25040118_top__DOT__inst))
                               ? (0xfffffffeU & vlSelf->ysyx_25040118_top__DOT__exu__DOT____VdfgTmp_h623bd169__0)
                               : (((0x63U == (0x7fU 
                                              & vlSelf->ysyx_25040118_top__DOT__inst))
                                    ? ((0U == (7U & 
                                               (vlSelf->ysyx_25040118_top__DOT__inst 
                                                >> 0xcU)))
                                        ? (vlSelf->ysyx_25040118_top__DOT__reg_src1 
                                           == vlSelf->ysyx_25040118_top__DOT__reg_src2)
                                        : (IData)((
                                                   (0x1000U 
                                                    == 
                                                    (0x7000U 
                                                     & vlSelf->ysyx_25040118_top__DOT__inst)) 
                                                   & (vlSelf->ysyx_25040118_top__DOT__reg_src1 
                                                      != vlSelf->ysyx_25040118_top__DOT__reg_src2))))
                                    : ((0x63U == (0x7fU 
                                                  & vlSelf->ysyx_25040118_top__DOT__inst)) 
                                       & ((4U == (7U 
                                                  & (vlSelf->ysyx_25040118_top__DOT__inst 
                                                     >> 0xcU)))
                                           ? VL_LTS_III(32, vlSelf->ysyx_25040118_top__DOT__reg_src1, vlSelf->ysyx_25040118_top__DOT__reg_src2)
                                           : ((5U == 
                                               (7U 
                                                & (vlSelf->ysyx_25040118_top__DOT__inst 
                                                   >> 0xcU)))
                                               ? VL_GTES_III(32, vlSelf->ysyx_25040118_top__DOT__reg_src1, vlSelf->ysyx_25040118_top__DOT__reg_src2)
                                               : ((2U 
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
                                   : ((IData)(4U) + vlSelf->ysyx_25040118_top__DOT__pc)))));
        } else if (VL_UNLIKELY(((0x6fU == (0x7fU & vlSelf->ysyx_25040118_top__DOT__inst)) 
                                | (0x67U == (0x7fU 
                                             & vlSelf->ysyx_25040118_top__DOT__inst))))) {
            VL_WRITEF("[EXU] Jump at pc=0x%08x, target=0x%08x\n",
                      32,vlSelf->ysyx_25040118_top__DOT__pc,
                      32,((0x6fU == (0x7fU & vlSelf->ysyx_25040118_top__DOT__inst))
                           ? vlSelf->ysyx_25040118_top__DOT__exu__DOT____VdfgTmp_h8a6dd40b__0
                           : ((0x67U == (0x7fU & vlSelf->ysyx_25040118_top__DOT__inst))
                               ? (0xfffffffeU & vlSelf->ysyx_25040118_top__DOT__exu__DOT____VdfgTmp_h623bd169__0)
                               : (((0x63U == (0x7fU 
                                              & vlSelf->ysyx_25040118_top__DOT__inst))
                                    ? ((0U == (7U & 
                                               (vlSelf->ysyx_25040118_top__DOT__inst 
                                                >> 0xcU)))
                                        ? (vlSelf->ysyx_25040118_top__DOT__reg_src1 
                                           == vlSelf->ysyx_25040118_top__DOT__reg_src2)
                                        : (IData)((
                                                   (0x1000U 
                                                    == 
                                                    (0x7000U 
                                                     & vlSelf->ysyx_25040118_top__DOT__inst)) 
                                                   & (vlSelf->ysyx_25040118_top__DOT__reg_src1 
                                                      != vlSelf->ysyx_25040118_top__DOT__reg_src2))))
                                    : ((0x63U == (0x7fU 
                                                  & vlSelf->ysyx_25040118_top__DOT__inst)) 
                                       & ((4U == (7U 
                                                  & (vlSelf->ysyx_25040118_top__DOT__inst 
                                                     >> 0xcU)))
                                           ? VL_LTS_III(32, vlSelf->ysyx_25040118_top__DOT__reg_src1, vlSelf->ysyx_25040118_top__DOT__reg_src2)
                                           : ((5U == 
                                               (7U 
                                                & (vlSelf->ysyx_25040118_top__DOT__inst 
                                                   >> 0xcU)))
                                               ? VL_GTES_III(32, vlSelf->ysyx_25040118_top__DOT__reg_src1, vlSelf->ysyx_25040118_top__DOT__reg_src2)
                                               : ((2U 
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
                                   : ((IData)(4U) + vlSelf->ysyx_25040118_top__DOT__pc)))));
        }
    } else {
        VL_WRITEF("[EXU] Reset...\n");
    }
    if (vlSelf->rst_n) {
        if ((((0x63U != (0x7fU & vlSelf->ysyx_25040118_top__DOT__inst)) 
              & ((0x23U != (0x7fU & vlSelf->ysyx_25040118_top__DOT__inst)) 
                 & (0U != (0x1fU & (vlSelf->ysyx_25040118_top__DOT__inst 
                                    >> 7U))))) & (0U 
                                                  != 
                                                  (0x1fU 
                                                   & (vlSelf->ysyx_25040118_top__DOT__inst 
                                                      >> 7U))))) {
            __Vdlyvval__ysyx_25040118_top__DOT__reg_file__DOT__reg_file__v0 
                = vlSelf->ysyx_25040118_top__DOT__exu_result;
            __Vdlyvset__ysyx_25040118_top__DOT__reg_file__DOT__reg_file__v0 = 1U;
            __Vdlyvdim0__ysyx_25040118_top__DOT__reg_file__DOT__reg_file__v0 
                = (0x1fU & (vlSelf->ysyx_25040118_top__DOT__inst 
                            >> 7U));
        }
        vlSelf->ysyx_25040118_top__DOT__pc = vlSelf->ysyx_25040118_top__DOT__next_pc;
    } else {
        __Vdlyvset__ysyx_25040118_top__DOT__reg_file__DOT__reg_file__v1 = 1U;
        vlSelf->ysyx_25040118_top__DOT__pc = 0x80000000U;
    }
    if ((1U & (~ (IData)(vlSelf->rst_n)))) {
        vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__unnamedblk1__DOT__i = 0x20U;
    }
    if ((0x23U == (0x7fU & vlSelf->ysyx_25040118_top__DOT__inst))) {
        if ((1U == (IData)(vlSelf->ysyx_25040118_top__DOT__mem_wmask))) {
            __Vdlyvval__ysyx_25040118_top__DOT__mem__DOT__dmem__v0 
                = (0xffU & vlSelf->ysyx_25040118_top__DOT__reg_src2);
            __Vdlyvset__ysyx_25040118_top__DOT__mem__DOT__dmem__v0 = 1U;
            __Vdlyvlsb__ysyx_25040118_top__DOT__mem__DOT__dmem__v0 = 0U;
            __Vdlyvdim0__ysyx_25040118_top__DOT__mem__DOT__dmem__v0 
                = (0x3ffU & (vlSelf->ysyx_25040118_top__DOT__mem_addr 
                             >> 2U));
        } else if ((3U == (IData)(vlSelf->ysyx_25040118_top__DOT__mem_wmask))) {
            __Vdlyvval__ysyx_25040118_top__DOT__mem__DOT__dmem__v1 
                = (0xffffU & vlSelf->ysyx_25040118_top__DOT__reg_src2);
            __Vdlyvset__ysyx_25040118_top__DOT__mem__DOT__dmem__v1 = 1U;
            __Vdlyvlsb__ysyx_25040118_top__DOT__mem__DOT__dmem__v1 = 0U;
            __Vdlyvdim0__ysyx_25040118_top__DOT__mem__DOT__dmem__v1 
                = (0x3ffU & (vlSelf->ysyx_25040118_top__DOT__mem_addr 
                             >> 2U));
        } else if ((0xfU == (IData)(vlSelf->ysyx_25040118_top__DOT__mem_wmask))) {
            __Vdlyvval__ysyx_25040118_top__DOT__mem__DOT__dmem__v2 
                = vlSelf->ysyx_25040118_top__DOT__reg_src2;
            __Vdlyvset__ysyx_25040118_top__DOT__mem__DOT__dmem__v2 = 1U;
            __Vdlyvdim0__ysyx_25040118_top__DOT__mem__DOT__dmem__v2 
                = (0x3ffU & (vlSelf->ysyx_25040118_top__DOT__mem_addr 
                             >> 2U));
        }
    }
    if (__Vdlyvset__ysyx_25040118_top__DOT__mem__DOT__dmem__v0) {
        vlSelf->ysyx_25040118_top__DOT__mem__DOT__dmem[__Vdlyvdim0__ysyx_25040118_top__DOT__mem__DOT__dmem__v0] 
            = (((~ ((IData)(0xffU) << (IData)(__Vdlyvlsb__ysyx_25040118_top__DOT__mem__DOT__dmem__v0))) 
                & vlSelf->ysyx_25040118_top__DOT__mem__DOT__dmem
                [__Vdlyvdim0__ysyx_25040118_top__DOT__mem__DOT__dmem__v0]) 
               | (0xffffffffULL & ((IData)(__Vdlyvval__ysyx_25040118_top__DOT__mem__DOT__dmem__v0) 
                                   << (IData)(__Vdlyvlsb__ysyx_25040118_top__DOT__mem__DOT__dmem__v0))));
    }
    if (__Vdlyvset__ysyx_25040118_top__DOT__mem__DOT__dmem__v1) {
        vlSelf->ysyx_25040118_top__DOT__mem__DOT__dmem[__Vdlyvdim0__ysyx_25040118_top__DOT__mem__DOT__dmem__v1] 
            = (((~ ((IData)(0xffffU) << (IData)(__Vdlyvlsb__ysyx_25040118_top__DOT__mem__DOT__dmem__v1))) 
                & vlSelf->ysyx_25040118_top__DOT__mem__DOT__dmem
                [__Vdlyvdim0__ysyx_25040118_top__DOT__mem__DOT__dmem__v1]) 
               | (0xffffffffULL & ((IData)(__Vdlyvval__ysyx_25040118_top__DOT__mem__DOT__dmem__v1) 
                                   << (IData)(__Vdlyvlsb__ysyx_25040118_top__DOT__mem__DOT__dmem__v1))));
    }
    if (__Vdlyvset__ysyx_25040118_top__DOT__mem__DOT__dmem__v2) {
        vlSelf->ysyx_25040118_top__DOT__mem__DOT__dmem[__Vdlyvdim0__ysyx_25040118_top__DOT__mem__DOT__dmem__v2] 
            = __Vdlyvval__ysyx_25040118_top__DOT__mem__DOT__dmem__v2;
    }
    if (__Vdlyvset__ysyx_25040118_top__DOT__reg_file__DOT__reg_file__v0) {
        vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file[__Vdlyvdim0__ysyx_25040118_top__DOT__reg_file__DOT__reg_file__v0] 
            = __Vdlyvval__ysyx_25040118_top__DOT__reg_file__DOT__reg_file__v0;
    }
    if (__Vdlyvset__ysyx_25040118_top__DOT__reg_file__DOT__reg_file__v1) {
        vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file[1U] = 0U;
        vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file[2U] = 0U;
        vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file[3U] = 0U;
        vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file[4U] = 0U;
        vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file[5U] = 0U;
        vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file[6U] = 0U;
        vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file[7U] = 0U;
        vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file[8U] = 0U;
        vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file[9U] = 0U;
        vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file[0xaU] = 0U;
        vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file[0xbU] = 0U;
        vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file[0xcU] = 0U;
        vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file[0xdU] = 0U;
        vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file[0xeU] = 0U;
        vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file[0xfU] = 0U;
        vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file[0x10U] = 0U;
        vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file[0x11U] = 0U;
        vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file[0x12U] = 0U;
        vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file[0x13U] = 0U;
        vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file[0x14U] = 0U;
        vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file[0x15U] = 0U;
        vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file[0x16U] = 0U;
        vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file[0x17U] = 0U;
        vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file[0x18U] = 0U;
        vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file[0x19U] = 0U;
        vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file[0x1aU] = 0U;
        vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file[0x1bU] = 0U;
        vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file[0x1cU] = 0U;
        vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file[0x1dU] = 0U;
        vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file[0x1eU] = 0U;
        vlSelf->ysyx_25040118_top__DOT__reg_file__DOT__reg_file[0x1fU] = 0U;
    }
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

void Vysyx_25040118_top___024root___eval_nba(Vysyx_25040118_top___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vysyx_25040118_top__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vysyx_25040118_top___024root___eval_nba\n"); );
    // Body
    if (vlSelf->__VnbaTriggered.at(0U)) {
        Vysyx_25040118_top___024root___nba_sequent__TOP__0(vlSelf);
        vlSelf->__Vm_traceActivity[1U] = 1U;
    }
}

void Vysyx_25040118_top___024root___eval_triggers__act(Vysyx_25040118_top___024root* vlSelf);
#ifdef VL_DEBUG
VL_ATTR_COLD void Vysyx_25040118_top___024root___dump_triggers__act(Vysyx_25040118_top___024root* vlSelf);
#endif  // VL_DEBUG
#ifdef VL_DEBUG
VL_ATTR_COLD void Vysyx_25040118_top___024root___dump_triggers__nba(Vysyx_25040118_top___024root* vlSelf);
#endif  // VL_DEBUG

void Vysyx_25040118_top___024root___eval(Vysyx_25040118_top___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vysyx_25040118_top__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vysyx_25040118_top___024root___eval\n"); );
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
            Vysyx_25040118_top___024root___eval_triggers__act(vlSelf);
            if (vlSelf->__VactTriggered.any()) {
                vlSelf->__VactContinue = 1U;
                if (VL_UNLIKELY((0x64U < vlSelf->__VactIterCount))) {
#ifdef VL_DEBUG
                    Vysyx_25040118_top___024root___dump_triggers__act(vlSelf);
#endif
                    VL_FATAL_MT("/home/pz40/ysyx-workbench/npc/vsrc/ysyx_25040118_top.v", 1, "", "Active region did not converge.");
                }
                vlSelf->__VactIterCount = ((IData)(1U) 
                                           + vlSelf->__VactIterCount);
                __VpreTriggered.andNot(vlSelf->__VactTriggered, vlSelf->__VnbaTriggered);
                vlSelf->__VnbaTriggered.set(vlSelf->__VactTriggered);
                Vysyx_25040118_top___024root___eval_act(vlSelf);
            }
        }
        if (vlSelf->__VnbaTriggered.any()) {
            __VnbaContinue = 1U;
            if (VL_UNLIKELY((0x64U < __VnbaIterCount))) {
#ifdef VL_DEBUG
                Vysyx_25040118_top___024root___dump_triggers__nba(vlSelf);
#endif
                VL_FATAL_MT("/home/pz40/ysyx-workbench/npc/vsrc/ysyx_25040118_top.v", 1, "", "NBA region did not converge.");
            }
            __VnbaIterCount = ((IData)(1U) + __VnbaIterCount);
            Vysyx_25040118_top___024root___eval_nba(vlSelf);
        }
    }
}

#ifdef VL_DEBUG
void Vysyx_25040118_top___024root___eval_debug_assertions(Vysyx_25040118_top___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vysyx_25040118_top__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vysyx_25040118_top___024root___eval_debug_assertions\n"); );
    // Body
    if (VL_UNLIKELY((vlSelf->clk & 0xfeU))) {
        Verilated::overWidthError("clk");}
    if (VL_UNLIKELY((vlSelf->rst_n & 0xfeU))) {
        Verilated::overWidthError("rst_n");}
}
#endif  // VL_DEBUG
