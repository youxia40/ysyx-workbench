AM_SRCS := riscv/npc/start.S \
           riscv/npc/trm.c \
           riscv/npc/ioe.c \
           riscv/npc/timer.c \
           riscv/npc/input.c \
           riscv/npc/cte.c \
           riscv/npc/trap.S \
           platform/dummy/vme.c \
           platform/dummy/mpe.c

CFLAGS    += -fdata-sections -ffunction-sections
LDSCRIPTS += $(AM_HOME)/scripts/linker.ld
LDFLAGS   += --defsym=_pmem_start=0x80000000 --defsym=_entry_offset=0x0
LDFLAGS   += --gc-sections -e _start

MAINARGS_MAX_LEN = 64
MAINARGS_PLACEHOLDER = The insert-arg rule in Makefile will insert mainargs here.
CFLAGS += -DMAINARGS_MAX_LEN=$(MAINARGS_MAX_LEN) -DMAINARGS_PLACEHOLDER=\""$(MAINARGS_PLACEHOLDER)"\"


insert-arg: image
	@if command -v python3 > /dev/null 2>&1; then \
		python3 $(AM_HOME)/tools/insert-arg.py $(IMAGE).bin $(MAINARGS_MAX_LEN) "$(MAINARGS_PLACEHOLDER)" "$(mainargs)"; \
	elif command -v python > /dev/null 2>&1; then \
		python $(AM_HOME)/tools/insert-arg.py $(IMAGE).bin $(MAINARGS_MAX_LEN) "$(MAINARGS_PLACEHOLDER)" "$(mainargs)"; \
	else \
		echo "Error: Python is required but not found. Please install Python."; \
		exit 127; \
	fi

image: image-dep
	@$(OBJDUMP) -d $(IMAGE).elf > $(IMAGE).txt
	@echo + OBJCOPY "->" $(IMAGE_REL).bin
	@$(OBJCOPY) -S --set-section-flags .bss=alloc,contents -O binary $(IMAGE).elf $(IMAGE).bin

run: insert-arg
	@echo "Preparing to run simulation..."
	@echo "Using NPC platform"
	
	# 环境检查
	@if [ -z "$(NPC_HOME)" ]; then \
		echo "Error: NPC_HOME environment variable not set!"; \
		echo "       Please set NPC_HOME to the root directory of your NPC project"; \
		echo "       Example: export NPC_HOME=\$$HOME/ysyx-workbench/npc"; \
		exit 1; \
	fi
	
	# 仿真器检查
	@NPC_EXEC="$(NPC_HOME)/build/riscv32e-npc"; \
	if [ ! -x "$$NPC_EXEC" ]; then \
		echo "Error: NPC simulator not found at $$NPC_EXEC"; \
		echo "       Please build the NPC simulator by running 'make' in $$NPC_HOME"; \
		exit 1; \
	fi; \
	
	# 运行仿真器
	@echo "Executing NPC simulator with binary: $(IMAGE).bin"
	@$$NPC_EXEC "$(IMAGE).bin"

.PHONY: insert-arg
