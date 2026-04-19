#include "map.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define IO_SPACE_MAX (32 * 1024 * 1024)
#define PAGE_SIZE 4096u
#define PAGE_MASK (PAGE_SIZE - 1u)

static uint8_t *io_space = NULL;
static uint8_t *p_space = NULL;

static inline uint32_t host_read(const void *addr, int len) {
  uint32_t ret = 0;
  assert(len >= 1 && len <= 4);
  memcpy(&ret, addr, (size_t)len);
  return ret;
}

static inline void host_write(void *addr, int len, uint32_t data) {
  assert(len >= 1 && len <= 4);
  memcpy(addr, &data, (size_t)len);
}

uint8_t* new_space(int size) {//分配IO映射空间，返回指向新空间的指针，要求size必须是页大小的整数倍
  //page aligned
  if (io_space == NULL) {
    init_map();
  }

  uint8_t *p = p_space;
  size = (size + (int)PAGE_MASK) & ~(int)PAGE_MASK;
  p_space += size;
  assert((size_t)(p_space - io_space) < IO_SPACE_MAX);
  return p;
}

void init_map(void) {
  if (io_space != NULL) {
    return;
  }

  io_space = (uint8_t *)malloc(IO_SPACE_MAX);
  assert(io_space != NULL);
  p_space = io_space;
}

static inline bool map_inside(const IOMap *map, uint32_t addr) {//检查访问地址是否在MMIO映射范围内
  return addr >= map->low && addr <= map->high;
}

int find_mapid_by_addr(IOMap *maps, int size, uint32_t addr) {//根据访问地址查找对应的MMIO映射返回ID
  int i;
  for (i = 0; i < size; i++) {
    if (map_inside(maps + i, addr)) {
      return i;
    }
  }
  return -1;
}

static void check_bound(IOMap *map, uint32_t addr) {//检查访问地址是否在MMIO映射范围内
  if (map == NULL) {
    assert(map != NULL);
  }

  assert(addr >= map->low && addr <= map->high);
}

static inline void invoke_callback(io_callback_t c, uint32_t offset, int len, bool is_write) {//调用MMIO设备的回调函数，通知设备访问事件
  if (c != NULL) {
    c(offset, len, is_write);
  }
}

uint32_t map_read(uint32_t addr, int len, IOMap *map) {
  //从IO映射空间读取数据,并调用回调函数准备数据
  assert(len >= 1 && len <= 4);
  check_bound(map, addr);

  uint32_t offset = addr - map->low;
  invoke_callback(map->callback, offset, len, false);
  return host_read(map->space + offset, len);
}

void map_write(uint32_t addr, int len, uint32_t data, IOMap *map) {
  assert(len >= 1 && len <= 4);
  check_bound(map, addr);

  uint32_t offset = addr - map->low;
  host_write(map->space + offset, len, data);
  invoke_callback(map->callback, offset, len, true);
}
