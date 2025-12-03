# NPC Makefile
TOPNAME = ysyx_25040118_top
BUILD_DIR = ./build
OBJ_DIR = $(BUILD_DIR)/obj_dir
NPC_BIN = $(BUILD_DIR)/$(TOPNAME)
WAVEFORM_FILE = $(BUILD_DIR)/waveform.fst

# 获取项目根目录绝对路径
PROJECT_HOME = $(abspath .)
INCLUDE_DIR = $(PROJECT_HOME)/include
VSRC_DIR = $(PROJECT_HOME)/vsrc
CSRC_DIR = $(PROJECT_HOME)/csrc

# LLVM配置
LLVM_PREFIX ?= $(shell llvm-config --prefix 2>/dev/null || echo /usr/lib/llvm-14)
LLVM_INCLUDEDIR = $(LLVM_PREFIX)/include
LLVM_LIBDIR = $(LLVM_PREFIX)/lib
LLVM_VERSION = $(shell llvm-config --version 2>/dev/null || echo 14)

LLVM_LDFLAGS = $(shell llvm-config --ldflags --libs 2>/dev/null)
ifeq ($(LLVM_LDFLAGS),)
LLVM_LDFLAGS = -L$(LLVM_LIBDIR) -lLLVMCore -lLLVMSupport -lLLVMBitReader -lLLVMBitWriter -lLLVMIRReader
endif

# C++编译配置
CXX = ccache g++
CXXFLAGS = -std=c++14 -O2 -faligned-new -fcf-protection=none \
           -I$(LLVM_INCLUDEDIR) \
           -I$(INCLUDE_DIR) \
           -Wno-deprecated-declarations -Wno-unknown-pragmas

# 链接选项
LDFLAGS = $(LLVM_LDFLAGS) \
          -lreadline -lhistory -lncurses \
          -lz -lpthread -lm -ldl

# 源文件（使用绝对路径）
VSRCS = $(wildcard $(VSRC_DIR)/*.v)
CSRCS = $(wildcard $(CSRC_DIR)/*.cpp)

# Verilator配置
VERILATOR = verilator
VERILATOR_FLAGS = --cc --trace-fst --exe --build -sv \
                -Wno-UNUSEDSIGNAL -Wno-SYNCASYNCNET -Wno-PINCONNECTEMPTY \
                -Wno-WIDTHTRUNC -Wno-PINMISSING -Wno-IMPLICIT -Wno-UNDRIVEN \
                -Wno-VARHIDDEN -Wno-WIDTHEXPAND -Wno-CASEOVERLAP -Wno-CASEX \
                --top-module $(TOPNAME) \
                --LDFLAGS "$(LDFLAGS)"

# 默认目标
default: $(NPC_BIN)
	@echo "NPC仿真器构建完成: $(NPC_BIN)"

$(shell mkdir -p $(BUILD_DIR) $(OBJ_DIR))

# 构建NPC仿真器
$(NPC_BIN): $(VSRCS) $(CSRCS)
	@echo "开始编译，中间文件目录: $(OBJ_DIR)"
	$(VERILATOR) $(VERILATOR_FLAGS) \
		--Mdir $(OBJ_DIR) \
		-CFLAGS "$(CXXFLAGS)" \
		$(VSRCS) $(CSRCS) \
		--exe -o $(abspath $(NPC_BIN))
	@echo "可执行文件位置: $(NPC_BIN)"

# 运行仿真
run: $(NPC_BIN)
	@echo "运行NPC仿真器: ARGS=$(ARGS) IMG=$(IMG)"
	@$(NPC_BIN) $(ARGS) $(IMG)

# 查看波形
wave: $(WAVEFORM_FILE)
	gtkwave $(WAVEFORM_FILE)

# 清理
clean:
	rm -rf $(BUILD_DIR)
	rm -f waveform.fst

.PHONY: default run wave clean