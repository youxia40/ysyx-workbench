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

#include "local-include/reg.h"
#include <cpu/cpu.h>
#include <cpu/ifetch.h>
#include <cpu/decode.h>

#define R(i) gpr(i)
#define Mr vaddr_read
#define Mw vaddr_write

enum {                                                  //指令类别
  TYPE_I, TYPE_U, TYPE_S,
  TYPE_N, TYPE_J, TYPE_R,
  TYPE_B,// none
};

#define src1R() do { *src1 = R(rs1); } while (0)                                  //源操作数1
#define src2R() do { *src2 = R(rs2); } while (0)                                  //源操作数2
//immI等辅助宏, 用于从指令中抽取出立即数
#define immI() do { *imm = SEXT(BITS(i, 31, 20), 12); } while(0)                  //I型立即数
#define immU() do { *imm = SEXT(BITS(i, 31, 12), 20) << 12; } while(0)            //U型立即数
#define immS() do { *imm = (SEXT(BITS(i, 31, 25), 7) << 5) | BITS(i, 11, 7); } while(0)// //S型立即数
#define immJ() do { *imm = SEXT( (BITS(i, 31, 31) << 20) |  /* 符号位 */ \
                                  (BITS(i, 30, 21) << 1) |  /* 高位域 */ \
                                  (BITS(i, 20, 20) << 11) |  /* 中间位 */ \
                                  (BITS(i, 19, 12) << 12),    /* 低位域 */ 21); } while(0)     //J型立即数
#define immB() do { *imm = SEXT( (BITS(i, 31, 31) << 12) |  /* 符号位 */ \
                                  (BITS(i, 7, 7) << 11) |    /* 高位域 */ \
                                  (BITS(i, 30, 25) << 5) |   /* 中间位 */ \
                                  (BITS(i, 11, 8) << 1),     /* 低位域 */ 13); } while(0)           //B型立即数

static void decode_operand(Decode *s, int *rd, word_t *src1, word_t *src2, word_t *imm, int type) {           //解码操作数
                                         //rd,         src1,         src2    和    imm,  分别代表目的操作数的寄存器号码, 两个源操作数和立即数.
  uint32_t i = s->isa.inst;                                                               //取出指令
  int rs1 = BITS(i, 19, 15); 
  int rs2 = BITS(i, 24, 20);
  *rd     = BITS(i, 11, 7);                                                     //对目标操作数进行寄存器操作数的译码
  switch (type) {                                                         //根据指令类型进行操作数的译码
    case TYPE_I: src1R();          immI(); break;
    case TYPE_U:                   immU(); break;
    case TYPE_S: src1R(); src2R(); immS(); break;
    case TYPE_J:                   immJ(); break;
    case TYPE_R: src1R(); src2R();         break;
    case TYPE_B: src1R(); src2R(); immI(); break;
    case TYPE_N:                           break;                                   //无操作数
    default: panic("unsupported type = %d", type);
  }
}

static int decode_exec(Decode *s) {                                       //译码函数，指定指令中opcode
  s->dnpc = s->snpc;

#define INSTPAT_INST(s) ((s)->isa.inst)                 //NEMU取指令的时候会把指令记录到s->isa.inst中
#define INSTPAT_MATCH(s, name, type, ... /* execute body */ ) { \
  int rd = 0; \
  word_t src1 = 0, src2 = 0, imm = 0; \
  decode_operand(s, &rd, &src1, &src2, &imm, concat(TYPE_, type)); \
  __VA_ARGS__ ; \
}

  INSTPAT_START();                         //INSTPAT意思是instruction pattern，是一个宏(在nemu/include/cpu/decode.h中定义), 用于定义一条模式匹配规则
  //移位(0x1f的作用是为了限制移位的范围在0~31之间)
  INSTPAT("0000000 ????? ????? 001 ????? 01100 11", sll    , R, R(rd) = src1 << (src2 & 0x1f));        //左移
  INSTPAT("0000000 ????? ????? 001 ????? 00100 11", slli   , I, R(rd) = src1 << (imm & 0x1f));        //左移立即数
  INSTPAT("0000000 ????? ????? 101 ????? 01100 11", srl    , R, R(rd) = (uint32_t)src1 >> (src2 & 0x1f)); //右移
  INSTPAT("0000000 ????? ????? 101 ????? 00100 11", srli   , I, R(rd) = (uint32_t)src1 >> (imm & 0x1f)); //右移立即数
  INSTPAT("0100000 ????? ????? 101 ????? 01100 11", sra    , R, R(rd) = src1 >> (src2 & 0x1f));        //算术右移
  INSTPAT("0100000 ????? ????? 101 ????? 00100 11", srai   , I, R(rd) = src1 >> (imm & 0x1f));        //算术右移立即数
  //算术
  INSTPAT("0000000 ????? ????? 000 ????? 01100 11", add    , R, R(rd) = src1 + src2);        //加法
  INSTPAT("??????? ????? ????? 000 ????? 00100 11", addi   , I, R(rd) = src1 + imm);        //加立即数
  INSTPAT("0100000 ????? ????? 000 ????? 01100 11", sub    , R, R(rd) = src1 - src2);        //减法
  INSTPAT("??????? ????? ????? ??? ????? 00101 11", auipc  , U, R(rd) = s->pc + imm);   //pc加高位立即数
  INSTPAT("??????? ????? ????? ??? ????? 01101 11", lui    , U, R(rd) = imm);           //装入高位立即数
  //逻辑
  INSTPAT("0000000 ????? ????? 100 ????? 01100 11", xor    , R, R(rd) = src1 ^ src2);        //异或
  INSTPAT("??????? ????? ????? 100 ????? 00100 11", xori   , I, R(rd) = src1 ^ imm);        //异或立即数
  INSTPAT("0000000 ????? ????? 110 ????? 01100 11", or     , R, R(rd) = src1 | src2);        //或
  INSTPAT("??????? ????? ????? 110 ????? 00100 11", ori    , I, R(rd) = src1 | imm);        //或立即数
  INSTPAT("0000000 ????? ????? 111 ????? 01100 11", and    , R, R(rd) = src1 & src2);        //与
  INSTPAT("??????? ????? ????? 111 ????? 00100 11", andi   , I, R(rd) = src1 & imm);        //与立即数
  //比较-置位
  INSTPAT("??????? ????? ????? 100 ????? 00000 11", lbu    , I, R(rd) = Mr(src1 + imm, 1));
  INSTPAT("??????? ????? ????? 000 ????? 01000 11", sb     , S, Mw(src1 + imm, 1, src2));
  //跳转并链接
  INSTPAT("??????? ????? ????? ??? ????? 11011 11", jal    , J, R(rd) = s->dnpc, s->dnpc = s->pc + imm); //跳转并链接
  //环境
  INSTPAT("0000000 00001 00000 000 00000 11100 11", ebreak , N, NEMUTRAP(s->pc, R(10)));   // R(10) is $a0环境断点

  INSTPAT("??????? ????? ????? ??? ????? ????? ??", inv    , N, INV(s->pc));            //inv:表示"若前面所有的模式匹配规则都无法成功匹配, 则将该指令视为非法指令
  INSTPAT_END();

  R(0) = 0; // reset $zero to 0

  return 0;
}

int isa_exec_once(Decode *s) {//执行单条指令
  s->isa.inst = inst_fetch(&s->snpc, 4);                //inst_fetch负责取指， 取指操作的本质是进行一次内存的访问
  return decode_exec(s);                                          //译码函数（根据指令的编码格式, 从取出的指令中识别出相应的opcode即可）
}
