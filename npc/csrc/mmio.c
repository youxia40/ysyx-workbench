#include "mmio.h"
#include "common.h"
#include "trace/dtrace.h"

#include <assert.h>
#include <stdio.h>

#define NR_MAP 16

static IOMap maps[NR_MAP] = {};
static int nr_map = 0;

static IOMap* fetch_mmio_map(uint32_t addr) {
  //根据访问地址查找对应的MMIO映射返回指针
  int mapid = find_mapid_by_addr(maps, nr_map, addr);
  return (mapid == -1 ? NULL : &maps[mapid]);
}

static void report_mmio_overlap(const char *name1, uint32_t l1, uint32_t r1,
    const char *name2, uint32_t l2, uint32_t r2) {
  printf("MMIO region %s@[0x%08x, 0x%08x] is overlapped with %s@[0x%08x, 0x%08x]\n",
      name1, l1, r1, name2, l2, r2);
  assert(0 && "MMIO overlap");
}


void add_mmio_map(const char *name, uint32_t addr, void *space, uint32_t len, io_callback_t callback) {
  //检查MMIO映射是否与物理内存或已有的MMIO映射重叠
  assert(nr_map < NR_MAP);

  uint32_t left = addr;
  uint32_t right = addr + len - 1;
  uint32_t pmem_left = MEM_BASE;
  uint32_t pmem_right = MEM_BASE + MEM_SIZE - 1;

  if (!(right < pmem_left || left > pmem_right)) {
    report_mmio_overlap(name, left, right, "pmem", pmem_left, pmem_right);
  }

  for (int i = 0; i < nr_map; i++) {
    if (!(right < maps[i].low || left > maps[i].high)) {
      report_mmio_overlap(name, left, right, maps[i].name, maps[i].low, maps[i].high);
    }
  }

  maps[nr_map] = (IOMap){
    .name = name,
    .low = addr,
    .high = addr + len - 1,
    .space = (uint8_t *)space,
    .callback = callback,
  };

  nr_map++;
}

int mmio_in_range(uint32_t addr) {//判断访问地址是否在MMIO映射范围内
  return fetch_mmio_map(addr) != NULL;
}

const char *mmio_map_name(uint32_t addr) {
  IOMap *map = fetch_mmio_map(addr);
  return map ? map->name : NULL;
}


uint32_t mmio_read(uint32_t addr, int len) {
  //访问MMIO设备的接口,会被npc_pmem_read/write调用
  IOMap *map = fetch_mmio_map(addr);

#if NPC_ENABLE_DTRACE
  if (npc_ctx.debug.dtrace_enabled) {
    dtrace_read(addr, len, map ? map->name : NULL);
  }
#endif

  return map_read(addr, len, map);
}

void mmio_write(uint32_t addr, int len, uint32_t data) {
  //写MMIO设备的接口
  IOMap *map = fetch_mmio_map(addr);

#if NPC_ENABLE_DTRACE
  if (npc_ctx.debug.dtrace_enabled) {
    dtrace_write(addr, len, data, map ? map->name : NULL);
  }
#endif

  map_write(addr, len, data, map);
}
