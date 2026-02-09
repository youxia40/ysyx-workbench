#include <common.h>                             //包含通用的头文件，定义了一些常用的宏和类型
#include <elf.h>
#include <generated/autoconf.h>
#include <device/map.h>
#include <device/mmio.h>

//---------------------------itrace---------------------------


#define INST_NUM 16                                      //环形缓冲区容量

typedef struct {
    word_t pc;                                               //程序计数器地址
    uint32_t inst;                                           //原始指令机器码
}InstBuf;

static InstBuf iringbuf[INST_NUM];                                   //指令环形缓冲区
static int cur_inst = 0;                                         //当前写入位置（下一个空槽）


void itrace_inst(word_t pc, uint32_t inst) {                                 //记录指令到环形缓冲区
    iringbuf[cur_inst].pc = pc;
    iringbuf[cur_inst].inst = inst;
    cur_inst = (cur_inst + 1) % INST_NUM;                               //环形递增:0 ~ INST_NUM-1循环
}


//home/pz40/ysyx-workbench/nemu/src/utils/filelist.mk规定，关闭trace功能后，disasm.c不会被编译，但itrace_display_inst仍然会被调用，导致链接错误
#if defined(CONFIG_ITRACE) || defined(CONFIG_IQUEUE)
void itrace_display_inst() {                                                               //显示指令环形缓冲区内容
    char line[64];                                                          //用于存储每行输出的字符串
    int error_pos = (cur_inst - 1 + INST_NUM) % INST_NUM;                //错误指令位置,当前指令位置减1, 由于cur_inst是下一个空槽, 所以需要减1来获取最后一条指令的位置
    
    int start = (error_pos - INST_NUM/2 + INST_NUM) % INST_NUM;             //计算起始位置, 从错误位置向前INST_NUM条指令开始遍历

    for (int i = 0; i < INST_NUM; ++i) {                                //遍历环形缓冲区
        int idx = (start - i) % INST_NUM;                    //计算当前索引位置, 从start位置开始向前遍历

        if (iringbuf[idx].pc == 0) {                        // 跳过未初始化条目
            continue;  
        }


        //生成地址和原始指令部分
        char *p = line;
        p += snprintf(p, sizeof(line), "%s" FMT_WORD ": %08x\t", (idx == error_pos) ? "-->" : "   ",iringbuf[idx].pc, iringbuf[idx].inst);
        
        //反汇编指令
        void disassemble(char *str, int size, uint64_t pc, uint8_t *code, int nbyte);
        disassemble(p, line + sizeof(line) - p, iringbuf[idx].pc,
                  (uint8_t*)&iringbuf[idx].inst, 4);
        
        //机器码
        uint8_t *code = (uint8_t*)&iringbuf[idx].inst;
        snprintf(line + strlen(line), sizeof(line) - strlen(line),
                " [%02x %02x %02x %02x]", code[0], code[1], code[2], code[3]);
        
        puts(line);
    }
}
#else
void itrace_display_inst() {
}
#endif



//---------------------------mtrace---------------------------


#ifdef CONFIG_MTRACE

#include <stdio.h>
#include <stdbool.h>

static bool mtrace_enabled = false;  //全局开关

//启用/禁用追踪
void mtrace_enable(bool enable) {
    mtrace_enabled = enable;
}

//内存读追踪（直接打印）
void mtrace_read(uint32_t addr, int len) {
    if (mtrace_enabled) {
        printf("[mtrace] READ  0x%08x, len=%d\n", addr, len);
    }
}

//内存写追踪（直接打印）
void mtrace_write(uint32_t addr, int len, uint32_t data) {
    if (mtrace_enabled) {
        printf("[mtrace] WRITE 0x%08x, len=%d, data=0x%08x\n", addr, len, data);
    }
}

#else

//禁用时生成空函数
void mtrace_enable(bool enable) {
    (void)enable; 
}
void mtrace_read(uint32_t addr, int len) {
    (void)addr; (void)len; 
}
void mtrace_write(uint32_t addr, int len, uint32_t data) {
    (void)addr; (void)len; (void)data; 
}
#endif




//---------------------------dtrace---------------------------
#ifdef CONFIG_DTRACE

#include <stdio.h>
#include <stdbool.h>

static bool dtrace_enabled = false;  //全局开关

//启用/禁用追踪
void dtrace_enable(bool enable) {
    dtrace_enabled = enable;
}


//内存读追踪(直接打印）
void dtrace_read(uint32_t addr, int len) {
    IOMap *map = fetch_mmio_map(addr);

    if (dtrace_enabled) {
        printf("[dtrace] READ  ")
        if (map != NULL) {
            printf("%08s\n", map->name ? map->name : "(null)");
        }
        else {
            printf("%08s\n", "pmem");
        }
        printf("0x%08x, len=%d\n", addr, len);
    }
}

//内存写追踪（直接打印）
void dtrace_write(uint32_t addr, int len, uint32_t data) {
    IOMap *map = fetch_mmio_map(addr);

     if (dtrace_enabled) {
        printf("[dtrace] WRITE ")
        if (map != NULL) {
            printf("%08s\n", map->name ? map->name : "(null)");
        }
        else {
            printf("%08s\n", "pmem");
        }
        printf("0x%08x, len=%d, data=0x%08x\n", addr, len, data);
    }
}

#else

//禁用时生成空函数
void dtrace_enable(bool enable) { (void)enable; }
void dtrace_read(uint32_t addr, int len) { (void)addr; (void)len; }
void dtrace_write(uint32_t addr, int len, uint32_t data) { (void)addr; (void)len; (void)data; }
#endif






//---------------------------ftrace---------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <elf.h>

#define MAX_SYMBOLS 512                                                             //最大符号数量

typedef struct {
    char name[64];
    uint32_t addr;
    Elf32_Xword size;
} Symbol;

static Symbol symbols[MAX_SYMBOLS];                                                 //符号表
static int symbol_count = 0;

//查找函数名
const char *find_function_name(uint32_t addr) {
    for (int i = 0; i < symbol_count; i++) {
        if (addr >= symbols[i].addr && addr < symbols[i].addr + symbols[i].size) {
            return symbols[i].name;
        }
    }
    return NULL;
}



void parse_elf(const char *elf_file) {                                                      //解析ELF文件
    if (!elf_file) return;
    
    FILE *fp = fopen(elf_file, "rb");
    if (!fp) {
        fprintf(stderr, "We can't open this ELF file : %s\n", elf_file);
        return;
    }

    // 读取ELF头
    Elf32_Ehdr ehdr;
    if (fread(&ehdr, sizeof(ehdr), 1, fp) != 1) {
        fprintf(stderr, "Failed to read ELF header...\n");
        fclose(fp);
        return;
    }

    // 验证ELF魔数
    if (memcmp(ehdr.e_ident, "\x7F" "ELF", 4) != 0) {
        fprintf(stderr, "Invalid ELF file: \n");
        fclose(fp);
        return;
    }

    // 加载节头表
    Elf32_Shdr *shdrs = malloc(ehdr.e_shnum * sizeof(Elf32_Shdr));
    if (!shdrs) {
        fprintf(stderr, "Failed to allocate section header table memory...\n");
        fclose(fp);
        return;
    }

    fseek(fp, ehdr.e_shoff, SEEK_SET);
    if (fread(shdrs, sizeof(Elf32_Shdr), ehdr.e_shnum, fp) != ehdr.e_shnum) {
        fprintf(stderr, "Failed to read the section header...\n");
        free(shdrs);
        fclose(fp);
        return;
    }

    // 获取节名称表
    Elf32_Shdr *shstr_shdr = &shdrs[ehdr.e_shstrndx];
    char *shstrtab = malloc(shstr_shdr->sh_size);
    if (!shstrtab) {
        fprintf(stderr, "Failed to allocate section name table memory...\n");
        free(shdrs);
        fclose(fp);
        return;
    }

    fseek(fp, shstr_shdr->sh_offset, SEEK_SET);                                         //读取节名称表
    if (fread(shstrtab, 1, shstr_shdr->sh_size, fp) != shstr_shdr->sh_size) {
        fprintf(stderr, "Failed to read section name table...\n");
        free(shdrs);
        free(shstrtab);
        fclose(fp);
        return;
    }

    //查找符号表(.symtab)和字符串表(.strtab)
    Elf32_Shdr *symtab_shdr = NULL;
    Elf32_Shdr *strtab_shdr = NULL;
    
    for (int i = 0; i < ehdr.e_shnum; i++) {
        const char *name = shstrtab + shdrs[i].sh_name;
        
        if (strcmp(name, ".symtab") == 0) {
            symtab_shdr = &shdrs[i];
        } else if (strcmp(name, ".strtab") == 0) {
            strtab_shdr = &shdrs[i];
        }
    }

    // 验证找到的关键表
    if (!symtab_shdr || !strtab_shdr) {
        fprintf(stderr, "No symbol or string table found...\n");
        goto cleanup;
    }

    // 读取字符串表内容
    char *strtab = malloc(strtab_shdr->sh_size);
    if (!strtab) {
        fprintf(stderr, "Failed to allocate string table memory...\n");
        goto cleanup;
    }

    fseek(fp, strtab_shdr->sh_offset, SEEK_SET);
    if (fread(strtab, 1, strtab_shdr->sh_size, fp) != strtab_shdr->sh_size) {
        fprintf(stderr, "Failed to read the string table...\n");
        free(strtab);
        goto cleanup;
    }

    // 读取符号表内容
    int sym_count = symtab_shdr->sh_size / sizeof(Elf32_Sym);
    Elf32_Sym *syms = malloc(symtab_shdr->sh_size);
    if (!syms) {
        fprintf(stderr, "Failed to allocate symbol table memory...\n");
        free(strtab);
        goto cleanup;
    }

    fseek(fp, symtab_shdr->sh_offset, SEEK_SET);                                        //读取符号表
    if (fread(syms, sizeof(Elf32_Sym), sym_count, fp) != sym_count) {
        fprintf(stderr, "Failed to read the symbol table...\n");
        free(strtab);
        free(syms);
        goto cleanup;
    }

    // 提取函数符号
    for (int i = 0; i < sym_count && symbol_count < MAX_SYMBOLS; i++) {
        unsigned char type = ELF32_ST_TYPE(syms[i].st_info);
        if (type == STT_FUNC && syms[i].st_value != 0) {
            const char *name = strtab + syms[i].st_name;
            
            // 复制函数名
            strncpy(symbols[symbol_count].name, name, sizeof(symbols[symbol_count].name) - 1);
            symbols[symbol_count].name[sizeof(symbols[symbol_count].name) - 1] = '\0';
            
            symbols[symbol_count].addr = syms[i].st_value;
            symbols[symbol_count].size = syms[i].st_size;
            
            printf("Locate the function: %-32s ADDR: 0x%08x SIZE: %lu\n",
                   symbols[symbol_count].name,
                   symbols[symbol_count].addr,
                   (unsigned long)symbols[symbol_count].size);                                      //打印函数信息
            
            symbol_count++;
        }
    }
    
    // 清理资源
    free(strtab);
    free(syms);

cleanup:
    free(shdrs);                                                                                //释放节头表内存
    free(shstrtab);                                                                                     //释放节名称表内存
    fclose(fp);
}



static int rec_depth = 1;  // 调用深度计数器

void display_call_func(word_t pc, word_t func_addr) {                                           //显示调用函数信息
    // 获取调用者和被调用者名称
    const char *caller = find_function_name(pc);                                                        // 获取调用者函数名
    const char *callee = find_function_name(func_addr);                                           // 获取被调用者函数名
    
    
    printf("0x%08x: ", pc);
    for (int i = 0; i < rec_depth; i++) printf("  "); // 基于深度的缩进
    rec_depth++;
    
    
    printf("call %s@0x%08x → %s\n", 
           caller ? caller : "unknown", 
           pc,
           callee ? callee : "unknown_func");
}

void display_ret_func(word_t pc) {                                                      //显示返回函数信息
    rec_depth--;                                                                                     // 在打印前减少深度
    
    const char *func_name = find_function_name(pc);
    
    printf("0x%08x: ", pc);
    for (int i = 0; i < rec_depth; i++) printf("  ");
    
    
    printf("ret  %s@0x%08x", 
           func_name ? func_name : "unknown", 
           pc);
    
    
    IFDEF(CONFIG_FTRACE_RETVAL, 
        printf(" [ret=0x%08x]", R(10));                                                     // R(10) 通常用于存储函数返回值
    );
    printf("\n");
}