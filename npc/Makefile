# NPC Makefile
TOPNAME = ysyx_25040118_top
BUILD_DIR = ./build
OBJ_DIR = $(BUILD_DIR)/obj_dir
NPC_BIN = $(BUILD_DIR)/$(TOPNAME)

PROJECT_HOME = $(abspath .)
INCLUDE_DIR = $(PROJECT_HOME)/include
VSRC_DIR = $(PROJECT_HOME)/vsrc
CSRC_DIR = $(PROJECT_HOME)/csrc

LLVM_PREFIX ?= $(shell llvm-config --prefix 2>/dev/null || echo /usr/lib/llvm-14)
LLVM_INCLUDEDIR = $(LLVM_PREFIX)/include
LLVM_LIBDIR = $(LLVM_PREFIX)/lib
LLVM_VERSION = $(shell llvm-config --version 2>/dev/null || echo 14)

LLVM_LDFLAGS = $(shell llvm-config --ldflags --libs 2>/dev/null)
ifeq ($(LLVM_LDFLAGS),)
LLVM_LDFLAGS = -L$(LLVM_LIBDIR) -lLLVMCore -lLLVMSupport -lLLVMBitReader -lLLVMBitWriter -lLLVMIRReader
endif

CXX = ccache g++
CXXFLAGS = -std=c++14 -O2 -faligned-new -fcf-protection=none \
           -I$(LLVM_INCLUDEDIR) \
           -I$(INCLUDE_DIR) \
           -Wno-deprecated-declarations -Wno-unknown-pragmas

LDFLAGS = $(LLVM_LDFLAGS) \
          -lreadline -lhistory -lncurses \
          -lz -lpthread -lm -ldl

VSRCS = $(wildcard $(VSRC_DIR)/*.v)
CSRCS = $(wildcard $(CSRC_DIR)/*.cpp)

VERILATOR = verilator
VERILATOR_FLAGS = --cc --trace-fst --exe --build -sv \
                -Wno-UNUSEDSIGNAL -Wno-SYNCASYNCNET -Wno-PINCONNECTEMPTY \
                -Wno-WIDTHTRUNC -Wno-PINMISSING -Wno-IMPLICIT -Wno-UNDRIVEN \
                -Wno-VARHIDDEN -Wno-WIDTHEXPAND -Wno-CASEOVERLAP -Wno-CASEX \
                --top-module $(TOPNAME) \
                --LDFLAGS "$(LDFLAGS)"

default: $(NPC_BIN)
	@echo "NPC built: $(NPC_BIN)"

$(shell mkdir -p $(BUILD_DIR) $(OBJ_DIR))

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

clean:
	rm -rf $(BUILD_DIR)

.PHONY: default run clean
