#ifndef MMIO_H
#define MMIO_H

#include <stdint.h>
#include "map.h"

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
//检查MMIO映射是否与物理内存或已有的MMIO映射重叠
void add_mmio_map(const char *name, uint32_t addr, void *space, uint32_t len, io_callback_t callback);

int mmio_in_range(uint32_t addr);//判断访问地址是否在MMIO映射范围内
const char *mmio_map_name(uint32_t addr);//返回对应的MMIO映射名称

uint32_t mmio_read(uint32_t addr, int len);
void mmio_write(uint32_t addr, int len, uint32_t data);

#ifdef __cplusplus
}
#endif

#endif
