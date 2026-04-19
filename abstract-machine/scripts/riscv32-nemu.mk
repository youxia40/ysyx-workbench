include $(AM_HOME)/scripts/isa/riscv.mk
include $(AM_HOME)/scripts/platform/nemu.mk
CFLAGS  += -DISA_H=\"riscv/riscv.h\"
COMMON_CFLAGS += -march=rv32im_zicsr -mabi=ilp32   #盖了riscv.mk中定义的CFLAGS，指定了RV32I指令集和ILP32 ABI
LDFLAGS       += -melf32lriscv                     #覆盖默认的链接器选项，指定生成32位RISC-V ELF文件

AM_SRCS += riscv/nemu/start.S \
           riscv/nemu/cte.c \
           riscv/nemu/trap.S \
           riscv/nemu/vme.c
