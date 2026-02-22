AM_SRCS := riscv/npc/start.S \
           riscv/npc/trm.c \
           riscv/npc/ioe.c \
           riscv/npc/timer.c \
           riscv/npc/gpu.c \
           riscv/npc/input.c \
           riscv/npc/cte.c \
           riscv/npc/trap.S \
           platform/dummy/vme.c \
           platform/dummy/mpe.c

CFLAGS    += -fdata-sections -ffunction-sections#让未使用函数/数据段可被链接器裁剪
CFLAGS    += -I$(AM_HOME)/am/src/platform/npc/include#补充NPC平台专用头文件搜索路径
CFLAGS    += -I$(AM_HOME)/am/src/riscv#复用riscv.h中的in/out访问函数
LDSCRIPTS += $(AM_HOME)/scripts/linker.ld#使用AM统一链接脚本确定内存布局
LDFLAGS   += --defsym=_pmem_start=0x80000000 --defsym=_entry_offset=0x0#固定程序装载基址和入口偏移
LDFLAGS   += --gc-sections -e _start#删除未引用段并指定启动符号

#NPCFLAGS += -b #启用批处理模式,避免自动进入SDB交互

MAINARGS_MAX_LEN = 64#预留给mainargs字符串的最大长度
MAINARGS_PLACEHOLDER = The insert-arg rule in Makefile will insert mainargs here.#用于后续在bin中定位参数占位串
CFLAGS += -DMAINARGS_MAX_LEN=$(MAINARGS_MAX_LEN)#把参数长度编译进目标程序
CFLAGS += '-DMAINARGS_PLACEHOLDER="$(MAINARGS_PLACEHOLDER)"'#把占位串编译进目标程序

insert-arg: image
	@python3 $(AM_HOME)/tools/insert-arg.py $(IMAGE).bin $(MAINARGS_MAX_LEN) "$(MAINARGS_PLACEHOLDER)" "$(mainargs)"

image: image-dep
	@$(OBJDUMP) -d $(IMAGE).elf > $(IMAGE).txt
	@echo + OBJCOPY "->" $(IMAGE_REL).bin
	@$(OBJCOPY) -S --set-section-flags .bss=alloc,contents -O binary $(IMAGE).elf $(IMAGE).bin

BLUE := \033[34m
RESET := \033[0m


run: insert-arg
	$(MAKE) -C $(NPC_HOME) ISA=$(ISA) run ARGS="$(NPCFLAGS)" IMG=$(IMAGE).bin


.PHONY: insert-arg