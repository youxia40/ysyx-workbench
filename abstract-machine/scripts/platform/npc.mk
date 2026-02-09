#npc.mk
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
CFLAGS    += -I$(AM_HOME)/am/src/platform/npc/include
LDSCRIPTS += $(AM_HOME)/scripts/linker.ld
LDFLAGS   += --defsym=_pmem_start=0x80000000 --defsym=_entry_offset=0x0
LDFLAGS   += --gc-sections -e _start
CFLAGS += -DMAINARGS_MAX_LEN=1
CFLAGS += -DMAINARGS_PLACEHOLDER="\"\""

#NPC参数配置
NPCFLAGS += -l $(abspath $(shell dirname $(IMAGE).elf))/npc-log.txt
NPCFLAGS += -b  #批处理模式

#跳过参数插入步骤
insert-arg: image
	@echo "Skipping argument insertion for NPC"

#objdump静态反汇编和objcopy生成二进制文件
image: image-dep
	@$(OBJDUMP) -d $(IMAGE).elf > $(IMAGE).txt
	@echo + OBJCOPY "->" $(IMAGE_REL).bin
	@$(OBJCOPY) -S --set-section-flags .bss=alloc,contents -O binary $(IMAGE).elf $(IMAGE).bin
#-O binary：输出格式是raw binary（裸数据）;-S：strip 掉符号/调试信息（减小体积）对.bin来说本来也不需要符号表


run: image
	@echo "===== NPC Simulation Start ====="
	@echo "Loading program: $(notdir $(IMAGE).elf)"
	@echo "ELF file path: $(abspath $(IMAGE).elf)"
	@echo "BIN file path: $(abspath $(IMAGE).bin)"
	@echo "Entry point: 0x80000000"
	@echo "Memory range: [0x80000000, 0x87ffffff]"
	@echo "Batch mode: ENABLED"
	@echo "NPC executable: $(NPC_HOME)/build/ysyx_25040118_top"
	@echo "NPC flags: $(NPCFLAGS) $(abspath $(IMAGE).elf)"
	@echo "Current directory: $(shell pwd)"
	@echo "================================"
	
	#检查NPC模拟器存在
	@if [ ! -f "$(NPC_HOME)/build/ysyx_25040118_top" ]; then \
		echo "ERROR: NPC simulator not found at $(NPC_HOME)/build/ysyx_25040118_top"; \
		echo "Please build the NPC simulator first"; \
		exit 1; \
	fi
	
	#检查ELF文件存在
	@if [ ! -f "$(abspath $(IMAGE).elf)" ]; then \
		echo "ERROR: ELF file not found at $(abspath $(IMAGE).elf)"; \
		echo "Files in build directory:"; \
		ls -la $(shell dirname $(abspath $(IMAGE).elf)) || true; \
		echo "Please check the build process"; \
		exit 1; \
	fi
	
	#调用NPC模拟器运行程序
	$(NPC_HOME)/build/ysyx_25040118_top $(NPCFLAGS) $(abspath $(IMAGE).elf)
	
	# 检查退出码
	@if [ $$? -eq 0 ]; then \
		echo "NPC simulation succeeded"; \
		exit 0; \
	else \
		echo "NPC simulation failed with exit code $$?"; \
		exit 1; \
	
	echo "===== NPC Simulation End ====="; 
	fi

.PHONY: insert-arg