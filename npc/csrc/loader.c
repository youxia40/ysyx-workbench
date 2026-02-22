#include "loader.h"
#include "mem.h" 
#include <elf.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>

//加载ELF文件到NPC内存，并记录入口地址
void npc_load_elf(NPCContext* ctx, const char* elf_path) {
#if NPC_ENABLE_ASSERT
    assert(ctx != NULL);
    assert(elf_path != NULL);
#endif


    //把ELF路径保存到ctx里，方便ftrace_init使用
    if (elf_path) {

        
        strncpy(ctx->elf_path, elf_path, sizeof(ctx->elf_path) - 1);//复制路径字符串，确保结尾
        ctx->elf_path[sizeof(ctx->elf_path) - 1] = '\0';
    } else {
        ctx->elf_path[0] = '\0';//如果路径无效则置空字符串
    }

    int fd = open(elf_path, O_RDONLY); //ELF文件描述符
    if (fd < 0) {
        perror("Failed to open ELF file");
        exit(1);
    }

    struct stat st; //文件元信息（用于获取文件大小）
    if (fstat(fd, &st) < 0) {
        perror("Failed to stat ELF file");
        close(fd);
        exit(1);
    }
    size_t size = st.st_size; //ELF文件总大小（字节）

    void* buf = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0); //ELF映射到只读内存
    if (buf == MAP_FAILED) {//mmap失败返回MAP_FAILED
        perror("Failed to mmap ELF file");
        close(fd);
        exit(1);
    }

    uint8_t* base = (uint8_t*)buf; //ELF镜像起始地址

    Elf32_Ehdr* ehdr = (Elf32_Ehdr*)base; //ELF头
#if NPC_ENABLE_ASSERT
    assert(size >= sizeof(Elf32_Ehdr));
    assert(ehdr->e_phoff < size);
#endif
    Elf32_Phdr* phdr = (Elf32_Phdr*)(base + ehdr->e_phoff); //程序头表起始地址


    //主循环会把PC初始化到此处
    ctx->entry = ehdr->e_entry;
    //这只记录入口，不立即改pc；真正生效由主循环在复位后提交路径驱动


    //处理PT_LOAD类型，把文件中对应段的数据写入内存模型，并清零BSS段
    for (int i = 0; i < ehdr->e_phnum; i++) {
        if (phdr[i].p_type == PT_LOAD) {
            uint32_t addr = phdr[i].p_paddr;  //目标物理地址
            uint32_t filesz = phdr[i].p_filesz; //文件中实际可拷贝长度
            uint32_t memsz = phdr[i].p_memsz;  //内存中段总长度（含BSS）
            uint32_t offset = phdr[i].p_offset; //段在ELF文件中的偏移
            //使用p_paddr是为了与当前NPC“物理地址直写内存模型”的装载方式保持一致


            //复制数据到内存
            for (uint32_t j = 0; j < filesz; j++) {
                //按字节写入可统一覆盖任意段起始地址,避免对齐限制
                npc_mem_write(addr + j, base[offset + j], 1);
            }

            //清零BSS段
            if (memsz > filesz) {
                //ELF约定中这部分在文件里不存在，但运行时必须视作全0
                for (uint32_t j = filesz; j < memsz; j++) {
                    npc_mem_write(addr + j, 0, 1);
                }
            }
        }
    }

    munmap(buf, size);//解除映射
    close(fd);

    printf("[NPC][LOADER] elf=%s,entry=0x%08X\n", elf_path, ctx->entry);
}

//镜像加载入口，识别ELF或原始BIN
void npc_load_image(NPCContext* ctx, const char* image_path) {
#if NPC_ENABLE_ASSERT
    assert(ctx != NULL);
    assert(image_path != NULL);
#endif
    int fd = open(image_path, O_RDONLY);//尝试打开文件以读取前4字节识别格式
    if (fd < 0) {
        perror("Failed to open image file");
        exit(1);
    }

    uint8_t magic[4] = {0};//读取前4字节用于识别ELF格式
    ssize_t nread = read(fd, magic, sizeof(magic));//读取文件前4字节到magic数组
    if (nread < 0) {
        perror("Failed to read image header");
        close(fd);
        exit(1);
    }
    close(fd);

    bool is_elf = (nread == 4 && magic[0] == 0x7f && magic[1] == 'E' && magic[2] == 'L' && magic[3] == 'F');
    //ELF文件以0x7f 'E' 'L' 'F'开头，如果前4字节匹配则视为ELF格式

    
    if (is_elf) {//如果是ELF格式
        //命中ELF后直接走段装载流程，BIN路径不再执行
        npc_load_elf(ctx, image_path);
        return;
    }

    struct stat st;//文件元信息（用于获取文件大小）
    if (stat(image_path, &st) < 0) {
        perror("Failed to stat BIN file");
        exit(1);
    }

    size_t size = st.st_size; //BIN字节数
    if (size > MEM_SIZE) {
        printf("[NPC][LOADER] bin too large:%zu bytes(MEM_SIZE=%u)\n", size, MEM_SIZE);
        exit(1);
    }

    fd = open(image_path, O_RDONLY);//重新打开文件以读取内容（之前的fd已因为读取头部而关闭）
    if (fd < 0) {
        perror("Failed to open BIN file");
        exit(1);
    }

    uint8_t *buf = (uint8_t *)mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
    //将BIN整体映射后逐字节写入内存模型
    if (buf == MAP_FAILED) {//mmap失败返回MAP_FAILED
        perror("Failed to mmap BIN file");
        close(fd);
        exit(1);
    }

    for (size_t i = 0; i < size; i++) {
    //按字节写入内存模型，避免对齐限制；地址从MEM_BASE开始连续映射
        npc_mem_write(MEM_BASE + (uint32_t)i, buf[i], 1);
    }

    munmap(buf, size);
    close(fd);

    ctx->entry = MEM_BASE; //原始BIN默认从MEM_BASE开始执行
    ctx->elf_path[0] = '\0';
    //RAW BIN没有符号信息，清空elf_path可避免ftrace误尝试按旧ELF反查符号

    printf("[NPC][LOADER] rawbin=%s,base=0x%08X,size=%zu\n", image_path, MEM_BASE, size);
}
