/***************************************************************************************
* Copyright (c) 2014-2024 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include <utils.h>
#include <device/map.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

/* http://en.wikibooks.org/wiki/Serial_Programming/8250_UART_Programming */
// NOTE: this is compatible to 16550

#define CH_OFFSET 0//串口数据寄存器偏移地址

static uint8_t *serial_base = NULL;//串口设备基地址指针

static void serial_stdin_init() {//非SDL模式下初始化终端为非阻塞输入，便于串口输入轮询
  int flags = fcntl(STDIN_FILENO, F_GETFL);
  if (flags >= 0) {
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
  }
}

 
static void serial_putc(char ch) {//串口发送字符
  MUXDEF(CONFIG_TARGET_AM, putch(ch), putc(ch, stderr));
}

static void serial_io_handler(uint32_t offset, int len, bool is_write) {//串口IO处理函数
  assert(len == 1);
  switch (offset) {
    /* We bind the serial port with the host stderr in NEMU. */
    case CH_OFFSET:
      if (is_write) serial_putc(serial_base[0]);//写操作，发送字符
      else {
        int ch = getchar();
        if (ch == EOF) {//EOF，标准输入流结束标志
          clearerr(stdin);//stdin是标准输入流，clearerr函数用于清除文件流的错误标志和EOF标志，以便下一次读取操作能够正常进行
          serial_base[0] = 0xff;
        } else {
          serial_base[0] = (uint8_t)ch;
        }
      }
      break;
    default: panic("do not support offset = %d", offset);
  }
}

void init_serial() {//初始化串口设备
  serial_base = new_space(8);//分配串口设备空间
  serial_stdin_init();
#ifdef CONFIG_HAS_PORT_IO
  add_pio_map ("serial", CONFIG_SERIAL_PORT, serial_base, 8, serial_io_handler);
#else
  add_mmio_map("serial", CONFIG_SERIAL_MMIO, serial_base, 8, serial_io_handler);//映射串口设备
#endif

}
