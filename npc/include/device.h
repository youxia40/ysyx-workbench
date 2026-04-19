#ifndef DEVICE_H
#define DEVICE_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

void init_device(void);
void device_update(void);

void init_serial(void);
void init_timer(void);
void init_vga(void);
void init_i8042(void);
void serial_feed_input(uint8_t ch);

void send_key(uint8_t scancode, bool is_keydown);
void vga_update_screen(void);

#ifdef __cplusplus
}
#endif

#endif
