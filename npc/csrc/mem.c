#include "mem.h"
#include "dpi.h"

static uint8_t mem[MEM_SIZE];//连续字节数组作为主存后端

//判断地址是否落在主存地址空间
static inline bool in_pmem(uint32_t addr) {
  return (addr >= MEM_BASE) && (addr < MEM_BASE + MEM_SIZE);
}

//按4字节对齐读取一个word(避免未对齐UB)
int npc_mem_read(uint32_t addr) {
  if (!in_pmem(addr)) {
    return 0;//越界读返回0
  }

  uint32_t aligned = addr & ~0x3u;//按word对齐
  uint32_t offset  = aligned - MEM_BASE;//转换为数组偏移
#if NPC_ENABLE_ASSERT
  assert(offset + sizeof(uint32_t) <= MEM_SIZE);
#endif

  uint32_t val = 0;
  memcpy(&val, mem + offset, sizeof(val));//避免未对齐读取UB
  return (int)val;
}

//按字节掩码写主存
void npc_mem_write(uint32_t addr, uint32_t data, uint8_t mask) {
  if (!in_pmem(addr)) {
    return;//越界写直接丢弃
  }

  uint32_t aligned = addr & ~0x3u;//对齐到word基址
  uint32_t offset = aligned - MEM_BASE;//转换为数组偏移
  uint32_t lane = addr & 0x3u;//字节通道偏移
#if NPC_ENABLE_ASSERT
  assert(offset + sizeof(uint32_t) <= MEM_SIZE);
#endif


  uint8_t shifted_mask = (uint8_t)(mask << lane);//把掩码平移到目标字节通道
  uint32_t shifted_data = data << (lane * 8);//把写数据平移到目标字节位置
  //平移后统一按p[0..3]落盘，可把“非对齐地址 + 掩码写”规约为固定4字节窗口更新

  uint8_t *p = mem + offset;//当前word首地址


  //按掩码更新对应字节,未被mask选中的字节保持不变
  if (shifted_mask & 0x1) {
    p[0] = (uint8_t)((shifted_data >> 0)  & 0xff);
  }
  if (shifted_mask & 0x2) {
    p[1] = (uint8_t)((shifted_data >> 8)  & 0xff);
  }
  if (shifted_mask & 0x4) {
    p[2] = (uint8_t)((shifted_data >> 16) & 0xff);
  }
  if (shifted_mask & 0x8) {
    p[3] = (uint8_t)((shifted_data >> 24) & 0xff);
  }
}

//读取当前寄存器镜像到用户提供的数组(regs[0..REG_NUM-1])
void npc_get_registers(uint32_t regs[REG_NUM]) {
#if NPC_ENABLE_ASSERT
  assert(regs != NULL);
#endif
  npc_get_regs(regs);//复用DPI侧寄存器快照接口
}
