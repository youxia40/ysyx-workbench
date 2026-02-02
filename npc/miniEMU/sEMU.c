#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "/home/pz40/ysyx-workbench/am-kernels/tests/am-tests/include/amtest.h"

#define mem_words 262144    //1024*1024/4,1MB
#define name "./vga.bin"      //写bin文件名
#define W 256
#define H 256
#define VGA_BASE 0x20000000u
#define VGA_PIXELS (W*H)//像素点数
#define VGA_SIZE (VGA_PIXELS*4)//字节数
#define MEM_BASE 0x00000000u

uint32_t M[mem_words];
uint64_t cycle = 0;

static uint32_t color_buf[H][W];//颜色缓冲区，像素块

int is_halt=0;
uint32_t PC=0x00000000u;
uint32_t next_PC=0;
uint32_t R[16] = {
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000
};

size_t loader(char *path){
    FILE *fp=fopen(path,"rb");
    if(fp==NULL){
        printf("file open error\n");
        exit(1);
    }
    size_t size=0;
    uint8_t buf[4];

    while(size<mem_words&&fread(buf,1,4,fp)==4){      //每次只读4个字节
        M[size]=(buf[0])|(buf[1]<<8)|(buf[2]<<16)|(buf[3]<<24);
        size++;
    }
    printf("loader结束: loaded %zu words into memory\n",size);
    fclose(fp);
    return size;
}

void patch_ebreak(uint32_t addr){
    if(addr>>2>=mem_words){
        printf("添加的ebreak越界:addr=0x%08x,addr>>2=%u,mem_words=%u\n",addr,addr>>2,mem_words);
        exit(1);
    }
    M[addr>>2]=0x00100073;
}

uint32_t GPR(int wen,uint32_t gpr,uint32_t value){
    if(gpr>=16){
        printf("寄存器编号越界:gpr=%u\n",gpr);
        exit(1);
    }
    if(wen==1&&gpr!=0x0){
        R[gpr]=value;
    }
    return R[gpr];
}



uint8_t mem_read8(uint32_t addr) {
    if(addr<MEM_BASE||addr>=MEM_BASE+(mem_words<<2)) {
        return 0;
    }
    uint32_t idx=(addr-MEM_BASE)>>2;
    uint32_t off=addr&3;
    return (M[idx]>>(8*off))&0xFF;
}

void mem_write8(uint32_t addr,uint8_t data) {
    if(addr<MEM_BASE||addr>=MEM_BASE+(mem_words<<2)) {
        return;
    }
    uint32_t idx=(addr-MEM_BASE)>>2;
    uint32_t off=addr&3;
    uint32_t shift=8*off;
    uint32_t mask=0xFFu<<shift;
    M[idx]=(M[idx]&~mask)|((uint32_t)data<<shift);
}

uint32_t mem_read32(uint32_t addr) {
    if (addr&3){
        return (uint32_t)mem_read8(addr)
             |((uint32_t)mem_read8(addr+1)<<8)
             |((uint32_t)mem_read8(addr+2)<<16)
             |((uint32_t)mem_read8(addr+3)<<24);
    }
    if(addr<MEM_BASE||addr>=MEM_BASE+(mem_words<<2)) {
        return 0;
    }
    return M[(addr-MEM_BASE)>>2];
}

void mem_write32(uint32_t addr,uint32_t data) {
    if (addr&3) {
        mem_write8(addr,data&0xFF);
        mem_write8(addr+1, (data>>8)&0xFF);
        mem_write8(addr+2, (data>>16)&0xFF);
        mem_write8(addr+3, (data>>24)&0xFF);
        return;
    }
    if(addr<MEM_BASE||addr>=MEM_BASE+(mem_words<<2)) {
        return;
    }
    M[(addr-MEM_BASE)>>2]=data;
}



uint32_t ifu(uint32_t pc){
    if (pc < MEM_BASE || pc >= MEM_BASE + (mem_words << 2)) {
        printf("指令地址越界: pc=0x%08x\n", pc);
        is_halt=1;
        return 0;
    }
    uint32_t idx=(pc-MEM_BASE)>>2;
    return M[idx];

}

uint32_t ifu_imm(uint32_t opcode,uint32_t funct_3,uint32_t funct_7,uint32_t inst){
    if(opcode==0x00000037){//U
        return (inst&0xfffff000)>>12;
    }
    else if(opcode==0x00000067||opcode==0x00000003||opcode==0x00000013){//I
        return (inst&0xfff00000)>>20;
    }
    else if(opcode==0x00000033&&funct_3==0x00000000&&funct_7==0x00000000){//R
        return 0;
    }
    else if(opcode==0x00000023){//S
        return ((inst&0x00000f80)>>7)|((inst&0xfe000000)>>20);
    }
    else{
        return 0;
    }
}

uint32_t sext(uint32_t imm){//符号扩展……
    uint32_t sign=imm>>11;
        if(sign==0x00000001){
            imm=imm|0xfffff000;
        }
    return imm;
}

void exu(uint32_t opcode,uint32_t funct_3,uint32_t funct_7,uint32_t inst,uint32_t rs1,uint32_t rs2,uint32_t rd,uint32_t imm){
    if(opcode==0x00000037){//lui
        GPR(1,rd,(imm<<12)&0xfffff000);
    }
    else if(opcode==0x00000067&&funct_3==0x00000000){//jalr
        uint32_t t=PC+4;
        next_PC=(GPR(0,rs1,0)+sext(imm))&~1;
        GPR(1,rd,t);
    }
    else if(opcode==0x00000003&&funct_3==0x00000002){//lw
        GPR(1,rd,(uint32_t)(int32_t)mem_read32(GPR(0,rs1,0)+sext(imm)));
    }
    else if(opcode==0x00000003&&funct_3==0x00000004){//lbu
        GPR(1,rd,(uint32_t)mem_read8(GPR(0,rs1,0)+sext(imm)));
    }
    else if(opcode==0x00000013&&funct_3==0x00000000){//addi
        GPR(1,rd,sext(imm)+GPR(0,rs1,0));
    }
    else if(opcode==0x00000033&&funct_3==0x00000000&&funct_7==0x00000000){//add
        GPR(1,rd,GPR(0,rs1,0)+GPR(0,rs2,0));
    }
    else if(opcode==0x00000023&&funct_3==0x00000000){//sb
        mem_write8(GPR(0,rs1,0)+sext(imm),(uint8_t)GPR(0,rs2,0));
    }
    else if(opcode==0x00000023&&funct_3==0x00000002){//sw
        if(GPR(0,rs1,0)+sext(imm)>=VGA_BASE&&GPR(0,rs1,0)+sext(imm)<VGA_SIZE+VGA_BASE){
            uint32_t off=GPR(0,rs1,0)+sext(imm)-VGA_BASE;//计算偏移地址
            if((off>>2)<VGA_PIXELS){
                uint32_t x=(off>>2)%W;
                uint32_t y=(off>>2)/W;
                color_buf[y][x] = GPR(0,rs2,0);
            }
            
        }
        else{
            mem_write32(GPR(0,rs1,0)+sext(imm),GPR(0,rs2,0));
        }
    }
    else {
        printf("未实现的指令: PC=0x%08x, inst=0x%08x, opcode=0x%02x, funct3=0x%x, funct7=0x%02x\n",PC, inst, opcode, funct_3, funct_7);
        is_halt = 1;
    }
}


void inst_cycle() {
    uint32_t inst=ifu(PC);//31~0
    cycle++;//指令执行周期

    if (is_halt) {
        return;
    }
    if(inst==0x00100073){//ebreak 
        is_halt=1;

        return;
    }

    uint32_t opcode=inst&0x0000007f;//6~0
    uint32_t rs1=(inst&0x000f8000)>>15;//19~15
    uint32_t rs2=(inst&0x01f00000)>>20;//24~20
    uint32_t funct_7=(inst&0xfe000000)>>25;//31~25
    uint32_t funct_3=(inst&0x00007000)>>12;//14~12
    uint32_t rd=(inst&0x00000f80)>>7;//11~7
    uint32_t imm=ifu_imm(opcode,funct_3,funct_7,inst);


    next_PC=PC+4;
    exu(opcode,funct_3,funct_7,inst,rs1,rs2,rd,imm);
    
    
    PC=next_PC;


    if(cycle%1==0){
        io_write(AM_GPU_FBDRAW, 0, 0, color_buf, W, H, false);//绘制到屏幕上
        io_write(AM_GPU_FBDRAW, 0, 0, NULL, 0, 0, true);//刷新屏幕  
    }
}




int main(){
    ioe_init();

    //用来防止图片执行完后自动退出
    /*size_t put_ebreak=*/loader(name);//写bin文件名

    /*
    if(put_ebreak<mem_words){
        patch_ebreak(0x00000db0);//写halt地址
    }
    else{
        printf("无法添加ebreak指令，程序过大，超出内存范围\n");
        exit(1);
    }
    */


    while(!is_halt){
        inst_cycle();
        
        if(is_halt==1){
            printf("程序结束,PC=0x%08x,a0=0x%08x\n",PC,R[10]);
        }
    }

    return (R[10] == 0x00000000)?printf("[HIT GOOD TRAP]\n"):printf("[HIT BAD TRAP]\n");
   
}