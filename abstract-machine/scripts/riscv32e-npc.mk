# 完全自定义配置（适配 AM 环境与 NPC 独立运行）
ARCH = riscv32e-npc
CROSS_COMPILE ?= riscv64-unknown-elf-

# 基础编译选项（保留原逻辑，明确 RV32E 架构特性）
CFLAGS   += -march=rv32e -mabi=ilp32e -ffreestanding -nostdlib
ASFLAGS  += -march=rv32e -mabi=ilp32e

# 链接选项（保留原符号定义，确保内存布局兼容 NPC）
LDFLAGS = -m elf32lriscv
LDFLAGS += -T $(AM_HOME)/scripts/linker.ld  # AM 环境标准链接脚本
LDFLAGS += --defsym=_pmem_start=0x80000000  # NPC 内存起始地址（与 NPC 仿真器约定）
LDFLAGS += --defsym=_entry_offset=0         # 入口偏移量（与 NPC 仿真器对齐）

# NPC 依赖库（仅保留 NPC 运行必需的基础库，避免冗余）
AM_SRCS += riscv/npc/libgcc/div.S \
           riscv/npc/libgcc/muldi3.S \
           riscv/npc/libgcc/multi3.c \
           riscv/npc/libgcc/ashldi3.c \
           riscv/npc/libgcc/unused.c

# 关键路径修正（严格对齐用户环境）
AM_HOME ?= $(HOME)/ysyx-workbench/abstract-machine
NPC_HOME ?= $(HOME)/ysyx-workbench/npc
NPC_EXEC ?= $(abspath $(NPC_HOME)/build/obj_dir/Vysyx_25040118_top) # NPC 仿真器可执行文件（用户编译产物）

# 显式定义构建规则（保留原逻辑，仅修正路径依赖）
$(IMAGE).bin: $(IMAGE).elf
	@echo "+ OBJCOPY -> $(IMAGE).bin"
	@$(OBJCOPY) -O binary $< $@

$(IMAGE).elf: $(LINK_FILES)
	@echo "+ LD -> $(IMAGE).elf"
	@$(LD) $(LDFLAGS) -o $@ $^  # 直接链接 AM 内核与 NPC 库

# NPC 运行目标（核心功能：独立运行测试程序）
run: $(IMAGE).bin
	@echo "Running test on NPC platform..."
	@echo "NPC Simulator Path: $(NPC_EXEC)"
	@echo "Test Binary: $(abspath $(IMAGE).bin)"
	@$(NPC_EXEC) "$(abspath $(IMAGE).bin)"; \
	status=$$?; \ 
	if [ $$status -eq 0 ]; then \
		echo "[$(ALL)] PASS"; \
	elif [ $$status -eq 1 ]; then \
		echo "[$(ALL)] FAIL (BAD TRAP)"; \
	elif [ $$status -eq 2 ]; then \
		echo "[$(ALL)] FAIL (TIMEOUT)"; \
	else \
		echo "[$(ALL)] FAIL (UNKNOWN)"; \
	fi