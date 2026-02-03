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

CXX = ccache g++#使用ccache加速重复编译
CXXFLAGS = -std=c++14 -O2 -faligned-new -fcf-protection=none \
			-I$(LLVM_INCLUDEDIR) \
			-I$(INCLUDE_DIR) \
        	-Wno-deprecated-declarations -Wno-unknown-pragmas
#-O2优化;-faligned-new:支持对齐new（LLVM/Verilator需要）;-fcf-protection=none：关掉CET（否则某些机器链接报错）
#-Wno:忽略LLVM/Verilator的历史遗留warning

#链接LLVM需要的库
LDFLAGS = $(LLVM_LDFLAGS) \
			-lreadline -lhistory -lncurses \
			-lz -lpthread -lm
#zlib:LLVM依赖;pthread:多线程;m/dl:Linux 标配

VSRCS = $(wildcard $(VSRC_DIR)/*.v)
CSRCS = $(wildcard $(CSRC_DIR)/*.cpp)

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

run: $(NPC_BIN)
	@echo "run npc: ARGS=$(ARGS) IMG=$(IMG)"
	@$(NPC_BIN) $(ARGS) $(IMG)

check:
	@echo "VERISON: "verilator --version
	@echo "LLVM Version: $(LLVM_VERSION)"
	@echo "LLVM Prefix: $(LLVM_PREFIX)"
	@echo "CXX: $(CXX)"
	@echo "CXXFLAGS: $(CXXFLAGS)"
	@echo "LDFLAGS: $(LDFLAGS)"
	@echo "-----------------------------------------------------------------"
	@echo "Verilog Source Files: $(VSRCS)"
	@echo "-----------------------------------------------------------------"
	@echo "-----------------Running Verilator lint check...-----------------"
	@echo "-----------------------------------------------------------------"
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
