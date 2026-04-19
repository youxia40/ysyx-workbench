#include "device.h"
#include "map.h"
#include "mmio.h"

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <termios.h>
#include <unistd.h>

/* http://en.wikibooks.org/wiki/Serial_Programming/8250_UART_Programming */
// NOTE: this is compatible to 16550

#define CH_OFFSET 0 //串口数据寄存器偏移地址
#define UART_RX_Q_LEN 1024

static uint8_t *serial_base = NULL; //串口设备基地址指针
static uint8_t uart_rx_q[UART_RX_Q_LEN]; //串口输入环形队列
static int uart_rx_head = 0;
static int uart_rx_tail = 0;
static uint8_t serial_rx_latch = 0xff;
static int serial_rx_latch_left = 0;
static struct termios stdin_term_orig;
static int stdin_term_saved = 0;

static inline int q_next(int idx) {
  return (idx + 1) % UART_RX_Q_LEN;
}

static inline void uart_enqueue(uint8_t ch) {
  int next = q_next(uart_rx_tail);
  if (next == uart_rx_head) {
    //队列满时覆盖最老输入，优先保留最新输入
    uart_rx_head = q_next(uart_rx_head);
  }
  uart_rx_q[uart_rx_tail] = ch;
  uart_rx_tail = next;
}

static inline uint8_t uart_dequeue(void) {
  if (uart_rx_head == uart_rx_tail) {
    return 0xff;
  }
  uint8_t ch = uart_rx_q[uart_rx_head];
  uart_rx_head = q_next(uart_rx_head);
  return ch;
}

static void serial_stdin_poll(void) {
  uint8_t buf[64];

  while (1) {
    ssize_t n = read(STDIN_FILENO, buf, sizeof(buf));
    if (n > 0) {
      for (ssize_t i = 0; i < n; i++) {
        uart_enqueue(buf[i]);
      }
      continue;
    }

    if (n == 0) {
      //EOF，当前无可读输入
      break;
    }

    if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR) {
      break;
    }

    //其他读错误按“当前无输入”处理
    break;
  }
}

static void serial_restore_term(void) {
  if (stdin_term_saved) {
    (void)tcsetattr(STDIN_FILENO, TCSANOW, &stdin_term_orig);
    stdin_term_saved = 0;
  }
}

static void serial_term_signal_handler(int sig) {
  serial_restore_term();
  signal(sig, SIG_DFL);
  raise(sig);
}

void serial_feed_input(uint8_t ch) {
  uart_enqueue(ch);
}

static void serial_stdin_init(void) {
  //初始化终端为非阻塞输入，便于串口输入轮询
  int flags = fcntl(STDIN_FILENO, F_GETFL);
  if (flags >= 0) {
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
  }

  // 交互终端启用近似raw模式，保留ISIG以支持Ctrl+C
  if (isatty(STDIN_FILENO)) {
    struct termios t;
    if (tcgetattr(STDIN_FILENO, &stdin_term_orig) == 0) {
      t = stdin_term_orig;
      t.c_lflag &= (tcflag_t)~(ICANON | ECHO);
      t.c_iflag &= (tcflag_t)~(IXON | ICRNL);
      // Force output processing on, otherwise '\n' may not return to
      // column 0 and logs look like they have many leading spaces.
      t.c_oflag |= (tcflag_t)(OPOST | ONLCR);
      //read调用在无输入时立即返回，而不是等待输入或超时
      t.c_cc[VMIN] = 0;
      t.c_cc[VTIME] = 0;

      if (tcsetattr(STDIN_FILENO, TCSANOW, &t) == 0) {
        stdin_term_saved = 1;
        (void)atexit(serial_restore_term);
        signal(SIGQUIT, serial_term_signal_handler);
        signal(SIGHUP, serial_term_signal_handler);
        signal(SIGTSTP, serial_term_signal_handler);
      }
    }
  }
}

static void serial_putc(char ch) {
  //串口发送字符
  putc(ch, stderr);
  fflush(stderr);
}

static void serial_io_handler(uint32_t offset, int len, bool is_write) {
  //串口IO处理函数
  assert(len == 1);
  switch (offset) {
    case CH_OFFSET:
      if (is_write) {
        //写操作，发送字符
        serial_putc((char)serial_base[0]);
      } else {
        // NPC对串口寄存器可能发生重复读；每个有效字节回放两次，避免丢半字符
        if (serial_rx_latch_left <= 0) {
          serial_stdin_poll();
          serial_rx_latch = uart_dequeue();
          serial_rx_latch_left = (serial_rx_latch == 0xff) ? 1 : 2;
        }
        serial_base[0] = serial_rx_latch;
        serial_rx_latch_left--;
      }
      break;

    default:
      assert(0 && "unsupported serial offset");
  }
}

void init_serial(void) {
  //初始化串口设备
  serial_base = new_space(8); //分配串口设备空间
  serial_stdin_init();
  add_mmio_map("serial", MMIO_SERIAL_ADDR, serial_base, 8, serial_io_handler); //映射串口设备
}
