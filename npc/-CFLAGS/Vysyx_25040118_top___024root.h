// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design internal header
// See Vysyx_25040118_top.h for the primary calling header

#ifndef VERILATED_VYSYX_25040118_TOP___024ROOT_H_
#define VERILATED_VYSYX_25040118_TOP___024ROOT_H_  // guard

#include "verilated.h"

class Vysyx_25040118_top__Syms;

class Vysyx_25040118_top___024root final : public VerilatedModule {
  public:

    // DESIGN SPECIFIC STATE
    VL_IN8(clk,0,0);
    VL_IN8(rst_n,0,0);
    CData/*4:0*/ ysyx_25040118_top__DOT__alu_ctrl;
    CData/*3:0*/ ysyx_25040118_top__DOT__mem_wmask;
    CData/*0:0*/ ysyx_25040118_top__DOT__exu__DOT__branch_taken;
    CData/*0:0*/ __Vtrigrprev__TOP__clk;
    CData/*0:0*/ __VactContinue;
    IData/*31:0*/ ysyx_25040118_top__DOT__pc;
    IData/*31:0*/ ysyx_25040118_top__DOT__next_pc;
    IData/*31:0*/ ysyx_25040118_top__DOT__inst;
    IData/*31:0*/ ysyx_25040118_top__DOT__imm;
    IData/*31:0*/ ysyx_25040118_top__DOT__exu_result;
    IData/*31:0*/ ysyx_25040118_top__DOT__reg_src1;
    IData/*31:0*/ ysyx_25040118_top__DOT__reg_src2;
    IData/*31:0*/ ysyx_25040118_top__DOT__mem_addr;
    IData/*31:0*/ ysyx_25040118_top__DOT__reg_file__DOT__unnamedblk1__DOT__i;
    IData/*31:0*/ ysyx_25040118_top__DOT__exu__DOT__alu_result;
    IData/*31:0*/ ysyx_25040118_top__DOT__exu__DOT____VdfgTmp_h8a6dd40b__0;
    IData/*31:0*/ ysyx_25040118_top__DOT__exu__DOT____VdfgTmp_h623bd169__0;
    IData/*31:0*/ __Vfunc_ysyx_25040118_top__DOT__ifu__DOT__pmem_read__0__Vfuncout;
    IData/*31:0*/ __VstlIterCount;
    IData/*31:0*/ __VactIterCount;
    VlUnpacked<IData/*31:0*/, 32> ysyx_25040118_top__DOT__reg_file__DOT__reg_file;
    VlUnpacked<IData/*31:0*/, 1024> ysyx_25040118_top__DOT__mem__DOT__dmem;
    VlUnpacked<CData/*0:0*/, 2> __Vm_traceActivity;
    VlTriggerVec<1> __VstlTriggered;
    VlTriggerVec<1> __VactTriggered;
    VlTriggerVec<1> __VnbaTriggered;

    // INTERNAL VARIABLES
    Vysyx_25040118_top__Syms* const vlSymsp;

    // CONSTRUCTORS
    Vysyx_25040118_top___024root(Vysyx_25040118_top__Syms* symsp, const char* v__name);
    ~Vysyx_25040118_top___024root();
    VL_UNCOPYABLE(Vysyx_25040118_top___024root);

    // INTERNAL METHODS
    void __Vconfigure(bool first);
} VL_ATTR_ALIGNED(VL_CACHE_LINE_BYTES);


#endif  // guard
