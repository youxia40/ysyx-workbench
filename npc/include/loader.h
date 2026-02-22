#ifndef LOADER_H
#define LOADER_H

#include "common.h"

void npc_load_elf(NPCContext* ctx, const char* elf_path);//按ELF段信息加载程序与入口
void npc_load_image(NPCContext* ctx, const char* image_path);//自动识别ELF或BIN并完成加载

#endif