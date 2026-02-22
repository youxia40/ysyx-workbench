#ifndef MMIO_H
#define MMIO_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MMIO_SERIAL_ADDR 0xa00003f8u//串口寄存器地址
#define MMIO_RTC_LO_ADDR 0xa0000048u//RTC低32位寄存器地址
#define MMIO_RTC_HI_ADDR 0xa000004cu//RTC高32位寄存器地址
#define MMIO_KBD_ADDR 0xa0000060u//键盘寄存器地址
#define MMIO_VGACTL_ADDR 0xa0000100u//GPU控制寄存器地址
#define MMIO_SYNC_ADDR   0xa0000104u//GPU同步寄存器地址
#define MMIO_FB_ADDR     0xa1000000u//帧缓冲起始地址

int mmio_in_range(uint32_t addr_aligned);//判断访问地址是否映射到设备空间
uint32_t mmio_read(uint32_t addr_aligned);//读取设备寄存器值
void mmio_write(uint32_t addr_aligned, uint32_t data, uint8_t mask);//按掩码写设备寄存器

#ifdef __cplusplus
}
#endif

#endif
