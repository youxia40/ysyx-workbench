#include "loader.h"
#include "common.h"
#include "mem.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <elf.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>

// 加载ELF文件
void npc_load_elf(NPCContext* ctx, const char* elf_path) {
    // 先把 ELF 路径保存到 ctx 里，方便 ftrace_init 使用
    if (elf_path) {
        // elf_path 最好在 NPCContext 里是一个类似 char elf_path[256] 的缓冲区
        strncpy(ctx->elf_path, elf_path, sizeof(ctx->elf_path) - 1);
        ctx->elf_path[sizeof(ctx->elf_path) - 1] = '\0';
    } else {
        ctx->elf_path[0] = '\0';
    }

    int fd = open(elf_path, O_RDONLY);
    if (fd < 0) {
        perror("Failed to open ELF file");
        exit(1);
    }

    struct stat st;
    if (fstat(fd, &st) < 0) {
        perror("Failed to stat ELF file");
        close(fd);
        exit(1);
    }
    size_t size = st.st_size;

    void* buf = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (buf == MAP_FAILED) {
        perror("Failed to mmap ELF file");
        close(fd);
        exit(1);
    }

    uint8_t* base = (uint8_t*)buf;

    Elf32_Ehdr* ehdr = (Elf32_Ehdr*)base;
    Elf32_Phdr* phdr = (Elf32_Phdr*)(base + ehdr->e_phoff);

    // 设置入口地址
    ctx->entry = ehdr->e_entry;

    // 加载程序段
    for (int i = 0; i < ehdr->e_phnum; i++) {
        if (phdr[i].p_type == PT_LOAD) {
            uint32_t addr   = phdr[i].p_paddr;   // 物理/虚拟基地址（在 ysyx 环境下一般等于 vaddr）
            uint32_t filesz = phdr[i].p_filesz;
            uint32_t memsz  = phdr[i].p_memsz;
            uint32_t offset = phdr[i].p_offset;

            // 复制数据到内存
            for (uint32_t j = 0; j < filesz; j++) {
                npc_mem_write(addr + j, base[offset + j], 1);
            }

            // 清零 BSS 段
            if (memsz > filesz) {
                for (uint32_t j = filesz; j < memsz; j++) {
                    npc_mem_write(addr + j, 0, 1);
                }
            }
        }
    }

    munmap(buf, size);
    close(fd);

    printf("Loaded %s, entry point: 0x%08X\n", elf_path, ctx->entry);
}
