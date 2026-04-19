#ifndef MAP_H
#define MAP_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void(*io_callback_t)(uint32_t offset, int len, bool is_write);

typedef struct {
  const char *name;
  uint32_t low;
  uint32_t high;
  uint8_t *space;
  io_callback_t callback;
} IOMap;

uint8_t* new_space(int size);
void init_map(void);

int find_mapid_by_addr(IOMap *maps, int size, uint32_t addr);

uint32_t map_read(uint32_t addr, int len, IOMap *map);
void map_write(uint32_t addr, int len, uint32_t data, IOMap *map);

#ifdef __cplusplus
}
#endif

#endif
