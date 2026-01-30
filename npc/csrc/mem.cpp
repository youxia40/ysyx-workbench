#include "mem.h"
#include "common.h"
#include "dpi.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>



//内存数组
static uint8_t mem[MEM_SIZE];




int npc_mem_read(uint32_t addr) {
    //检查地址是否在有效范围内
    if (addr < MEM_BASE || addr >= MEM_BASE + MEM_SIZE) {
        return 0;
    }
    
    //计算内存偏移量
    uint32_t offset = addr - MEM_BASE;
    
    //返回32位数据
    return *(uint32_t*)(mem + offset);
}



void npc_mem_write(uint32_t addr, uint32_t data, uint8_t mask) {
    //检查地址是否在有效范围内
    if (addr < MEM_BASE || addr >= MEM_BASE + MEM_SIZE) {
        return;
    }
    
    //计算内存偏移量
    uint32_t offset = addr - MEM_BASE;
    uint8_t *p = mem + offset;
    
    //根据掩码写入数据
    if (mask & 0x1) *p = data & 0xFF;
    if (mask & 0x2) *(p+1) = (data >> 8) & 0xFF;
    if (mask & 0x4) *(p+2) = (data >> 16) & 0xFF;
    if (mask & 0x8) *(p+3) = (data >> 24) & 0xFF;
}


//获取寄存器值
void npc_get_registers(uint32_t regs[16]) {

    //从CPU寄存器文件中读取真实值
    npc_get_regs(regs);
}
