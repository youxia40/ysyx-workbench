# NPC Makefile
TOPNAME=ysyx_25040118_top
BUILD_DIR=./build
OBJ_DIR=$(BUILD_DIR)/obj_dir
NPC_BIN=$(BUILD_DIR)/$(TOPNAME)

#综合评估相关
LIB?=/home/pz40/ysyx-workbench/npc/flow/syn/cell.lib
OSS_CAD?=$(HOME)/ysyx-workbench/oss-cad-suite
YOSYS_STA_HOME ?= $(HOME)/ysyx-workbench/yosys-sta
YOSYS?=$(OSS_CAD)/bin/yosys
export LIB YOSYS TOPNAME


PROJECT_HOME=$(abspath .)
#当前所在目录的绝对路径
INCLUDE_DIR=$(PROJECT_HOME)/include
VSRC_DIR=$(PROJECT_HOME)/vsrc
CSRC_DIR=$(PROJECT_HOME)/csrc


LLVM_PREFIX?=$(shell llvm-config --prefix 2>/dev/null || echo /usr/lib/llvm-14)
LLVM_INCLUDEDIR=$(LLVM_PREFIX)/include
LLVM_LIBDIR=$(LLVM_PREFIX)/lib
LLVM_VERSION=$(shell llvm-config --version 2>/dev/null || echo 14)

LLVM_LDFLAGS=$(shell llvm-config --ldflags --libs core support irreader bitreader bitwriter)
ifeq ($(LLVM_LDFLAGS),)
LLVM_LDFLAGS=-L$(LLVM_LIBDIR) -lLLVMCore -lLLVMSupport -lLLVMBitReader -lLLVMBitWriter -lLLVMIRReader
endif

#NPC平台特定源文件和编译链接配置
SDL2_CFLAGS := $(shell pkg-config --cflags sdl2 2>/dev/null)
SDL2_LIBS   := $(shell pkg-config --libs sdl2 2>/dev/null)
#如果SDL2不可用则禁用相关功能,并在编译时定义NPC_USE_SDL=0
ifeq ($(strip $(SDL2_LIBS)),)#如果SDL2不可用则禁用相关功能,并在编译时定义NPC_USE_SDL=0
	NPC_USE_SDL := 0
else
	NPC_USE_SDL := 1
endif


CXX = ccache g++#使用ccache加速重复编译
CXXFLAGS = -std=c++14 -O2 -faligned-new -fcf-protection=none \
			-I$(LLVM_INCLUDEDIR) \
			-I$(INCLUDE_DIR) \
	        	-Wno-deprecated-declarations -Wno-unknown-pragmas \
			-DNPC_USE_SDL=$(NPC_USE_SDL) \
			$(SDL2_CFLAGS)
#-O2优化;-faligned-new:支持对齐new（LLVM/Verilator需要）;-fcf-protection=none：关掉CET（否则某些机器链接报错）
#-Wno:忽略LLVM/Verilator的历史遗留warning

#链接LLVM需要的库
LDFLAGS = $(LLVM_LDFLAGS) \
			-lreadline -lhistory -lncurses \
			-lz -lpthread -lm \
			$(SDL2_LIBS)
#zlib:LLVM依赖;pthread:多线程;m/dl:Linux 标配

VSRCS = $(wildcard $(VSRC_DIR)/*.v)
CSRCS = \
	$(CSRC_DIR)/main.cpp \
	$(CSRC_DIR)/expr.cpp \
	$(CSRC_DIR)/ftrace.cpp \
	$(CSRC_DIR)/difftest.cpp \
	$(CSRC_DIR)/wave.cpp \
	$(wildcard $(CSRC_DIR)/*.c)

VERILATOR = verilator
VERILATOR_FLAGS = --cc --trace-fst --build -sv \
                -Wno-UNUSEDSIGNAL -Wno-SYNCASYNCNET -Wno-PINCONNECTEMPTY \
                -Wno-WIDTHTRUNC -Wno-PINMISSING -Wno-IMPLICIT -Wno-UNDRIVEN \
                -Wno-VARHIDDEN -Wno-WIDTHEXPAND -Wno-CASEOVERLAP -Wno-CASEX \
                --top-module $(TOPNAME) \
                --LDFLAGS "$(LDFLAGS)"
#把最终链接参数传给g++

default: $(NPC_BIN)
	@echo "NPC built: $(NPC_BIN)"

$(shell mkdir -p $(BUILD_DIR) $(OBJ_DIR))
#创建构建目录和对象目录

$(NPC_BIN): $(VSRCS) $(CSRCS)
	@echo "build dir: $(OBJ_DIR)"
	$(VERILATOR) $(VERILATOR_FLAGS) \
		--Mdir $(OBJ_DIR) \
		-CFLAGS "$(CXXFLAGS)" \
		$(VSRCS) $(CSRCS) \
		--exe -o $(abspath $(NPC_BIN))
	@echo "npc bin: $(NPC_BIN)"


BLUE = \033[34m
BLUE_END = \033[0m


run: $(NPC_BIN)
	@echo "$(BLUE)[NPC] built=$(NPC_BIN)$(BLUE_END)"
	@echo "$(BLUE)[NPC] run args='$(ARGS)' img='$(IMG)'$(BLUE_END)"
	@$(NPC_BIN) $(ARGS) $(IMG)

check:
	@echo "VERISON: "verilator --version
	@echo "$(BLUE)LLVM Version:$(BLUE_END) $(LLVM_VERSION)"
	@echo "$(BLUE)LLVM Prefix:$(BLUE_END) $(LLVM_PREFIX)"
	@echo "$(BLUE)CXX:$(BLUE_END) $(CXX)"
	@echo "$(BLUE)CXXFLAGS:$(BLUE_END) $(CXXFLAGS)"
	@echo "$(BLUE)LDFLAGS:$(BLUE_END) $(LDFLAGS)"
	@echo "$(BLUE)Verilog Source Files:$(BLUE_END) $(VSRCS)"
	@echo "$(BLUE)-----------------Running Verilator lint check...-----------------$(BLUE_END)"
	@echo "$(BLUE)-----------------------------------------------------------------$(BLUE_END)"
	verilator --lint-only  $(VSRCS)


syn:
	@mkdir -p build/syn
	$(YOSYS) -l build/syn/yosys.log -c flow/syn/syn.tcl
	@grep -n --color=always "Warning:" build/syn/yosys.log || echo -e "\033[32mNo warnings\033[0m"


sta:
	@mkdir -p build/sta
	$(MAKE) -C $(YOSYS_STA_HOME) sta \
		DESIGN=$(TOPNAME) \
		RTL_FILES="$(VSRCS)" \
		SDC_FILE=$(PROJECT_HOME)/flow/sta/constraints.sdc \
		CLK_PORT_NAME=clk \
		CLK_FREQ_MHZ=100 \
		O=$(PROJECT_HOME)/build/sta



clean:
	rm -rf $(BUILD_DIR)
.PHONY: default run clean check syn sta
