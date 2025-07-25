TOPNAME = ysyx_25040118_top
BUILD_DIR = ./build
OBJ_DIR = $(BUILD_DIR)/obj_dir
SIM_HOME = $(abspath .)
NEMU_HOME = /home/pz40/ysyx-workbench/nemu
AM_HOME = /home/pz40/ysyx-workbench/am-kernels

# LLVM 配置
LLVM_PREFIX ?= $(shell llvm-config --prefix 2>/dev/null || echo /usr/lib/llvm-14)
LLVM_INCLUDEDIR = $(LLVM_PREFIX)/include
LLVM_LIBDIR = $(LLVM_PREFIX)/lib
LLVM_VERSION = $(shell llvm-config --version 2>/dev/null || echo 14)

# 使用 llvm-config 获取完整的链接标志
LLVM_LDFLAGS = $(shell llvm-config --ldflags --libs 2>/dev/null)
ifeq ($(LLVM_LDFLAGS),)
LLVM_LDFLAGS = -L$(LLVM_LIBDIR) -lLLVMCore -lLLVMSupport -lLLVMBitReader -lLLVMBitWriter -lLLVMIRReader
endif

# 编译器配置
CXX = ccache g++
CXXFLAGS = -std=c++14 -Os -faligned-new -fcf-protection=none \
           -I$(LLVM_INCLUDEDIR) \
           -I$(SIM_HOME)/csrcn
CXXFLAGS += -Wno-deprecated-declarations -Wno-unknown-pragmas

# NEMU 共享库配置
NEMU_SHARED_LIB = $(NEMU_HOME)/build/riscv32-nemu-interpreter-so
ifeq ($(wildcard $(NEMU_SHARED_LIB)),)
$(error NEMU 共享库未找到: $(NEMU_HOME))
endif
NEMU_LDFLAGS = -L$(dir $(NEMU_SHARED_LIB)) -Wl,-rpath=$(dir $(NEMU_SHARED_LIB)) -l:$(notdir $(NEMU_SHARED_LIB))

# 综合链接标志（关键修复：显式包含所有必要库）
LDFLAGS = $(LLVM_LDFLAGS) $(NEMU_LDFLAGS) \
          -lreadline -lhistory -lncurses \
          -lz -lpthread -lm -ldl           # 包含 zlib 和其他基础库
LDFLAGS += -ldl  # 动态链接库支持（用于 NEMU 共享库）

# 源文件列表
VSRCS = $(wildcard $(SIM_HOME)/vsrc/*.v)
CSRCS = $(wildcard $(SIM_HOME)/csrcn/*.cpp)

# Verilator 配置（关键修复：显式传递 LDFLAGS）
VERILATOR = verilator
VERILATOR_FLAGS = --cc --trace-fst --exe --build \
                -Wno-UNUSEDSIGNAL -Wno-SYNCASYNCNET -Wno-PINCONNECTEMPTY \
                -Wno-WIDTHTRUNC -Wno-PINMISSING -Wno-IMPLICIT -Wno-UNDRIVEN \
                -Wno-VARHIDDEN -Wno-WIDTHEXPAND -Wno-CASEOVERLAP -Wno-CASEX \
                --top-module $(TOPNAME) \
                --LDFLAGS "$(LDFLAGS)"  # 显式传递链接标志

# 默认目标
.PHONY: default
default: $(OBJ_DIR)/Vysyx_25040118_top

# 生成 Verilator 中间文件（关键修复：直接生成可执行文件）
$(OBJ_DIR)/Vysyx_25040118_top: $(VSRCS) $(CSRCS)
	@mkdir -p $(OBJ_DIR)
	$(VERILATOR) $(VERILATOR_FLAGS) \
		--Mdir $(OBJ_DIR) \
		-CFLAGS "$(CXXFLAGS)" \
		$(SIM_HOME)/main.cpp \
		$(CSRCS) \
		$(VSRCS)

# 清理目标
.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)
	rm -f waveform.fst