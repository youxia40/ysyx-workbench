#ifndef NPC_H__
#define NPC_H__

#include <riscv.h>

#define DEVICE_BASE 0xa0000000u//NPC设备MMIO基址
#define MMIO_BASE   0xa0000000u//设备地址基址,所有设备寄存器地址都从这里开始偏移

#define SERIAL_PORT (DEVICE_BASE + 0x00003f8u)//串口地址
#define KBD_ADDR    (DEVICE_BASE + 0x0000060u)//键盘寄存器地址
#define RTC_ADDR    (DEVICE_BASE + 0x0000048u)//RTC寄存器地址(低32位),高32位在RTC_ADDR+4
#define VGACTL_ADDR (DEVICE_BASE + 0x0000100u)//GPU控制寄存器地址,读出屏幕宽高信息
#define SYNC_ADDR   (VGACTL_ADDR + 4u)//GPU刷新寄存器地址,写入1表示请求刷新,设备处理完后会清零
#define FB_ADDR     (0xa1000000u)//帧缓冲起始地址,从这里开始是连续的显存空间

#endif
