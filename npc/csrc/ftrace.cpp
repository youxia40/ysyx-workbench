#include "ftrace.h"
#include <elf.h>
#include <sys/stat.h>
#include <sys/mman.h>//ELF解析相关头文件
#include <fcntl.h>//文件操作
#include <unistd.h>//文件操作

typedef struct {
  uint32_t start;//函数起始地址
  uint32_t end;//函数结束地址(开区间上界)
  char name[64];//函数名
} FuncSym;//函数符号结构体

#define MAX_FUNCS 256//最大支持的函数数量
static FuncSym funcs[MAX_FUNCS];//函数符号表
static int func_cnt = 0;//已加载函数数量
static uint32_t ftrace_line_count = 0;//已输出行数

static void load_symbols(const char *path) {//从ELF符号表提取函数信息，后续用于call/ret地址反查函数名
  //从ELF符号表提取函数信息,后续用于call/ret地址反查函数名
  if (!path || path[0] == '\0') {
    return;
  }
  int fd = open(path, O_RDONLY);//打开ELF文件，O_RDONLY表示只读模式，位于<fcntl.h>
  if (fd < 0) {
    return;
  }

  struct stat st;//获取文件元信息，位于<sys/stat.h>
  if (fstat(fd, &st) < 0) {//获取文件大小等信息，fstat函数位于<sys/stat.h>
    close(fd);
    return;
  }

  size_t size = st.st_size;//文件大小
  void *buf = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
  //映射ELF到内存，PROT_READ表示只读，MAP_PRIVATE表示私有映射，位于<sys/mman.h>

  if (buf == MAP_FAILED) {//mmap失败返回MAP_FAILED，位于<sys/mman.h>
    close(fd);
    return;
  }
  uint8_t *base = (uint8_t *)buf;//ELF镜像起始地址
  Elf32_Ehdr *eh = (Elf32_Ehdr *)base;//ELF头
  if (eh->e_ident[EI_MAG0] != ELFMAG0 ||eh->e_ident[EI_MAG1] != ELFMAG1 ||
      eh->e_ident[EI_MAG2] != ELFMAG2 ||eh->e_ident[EI_MAG3] != ELFMAG3) {//检查ELF，位于<elf.h>
    munmap(buf, size);//解除映射，位于<sys/mman.h>
    close(fd);
    return;
  }
  Elf32_Shdr *sh = (Elf32_Shdr *)(base + eh->e_shoff);//节头表
  int shnum = eh->e_shnum;//节头数量
  Elf32_Shdr *sym_sh = NULL;//符号表节
  for (int i = 0; i < shnum; i++) {
    if (sh[i].sh_type == SHT_SYMTAB) {//优先使用SHT_SYMTAB类型的符号表，通常包含完整符号信息
      sym_sh = &sh[i];
      break;
    }
  }
  if (!sym_sh) {
    for (int i = 0; i < shnum; i++) {//如果没有SHT_SYMTAB，再尝试SHT_DYNSYM
      if (sh[i].sh_type == SHT_DYNSYM) {
        sym_sh = &sh[i];
        break;
      }
    }
  }
  if (!sym_sh) {
    munmap(buf, size);
    close(fd);
    return;
  }
  int str_idx = sym_sh->sh_link;//字符串表索引
  if (str_idx < 0 || str_idx >= shnum) {
    munmap(buf, size);
    close(fd);
    return;
  }
  Elf32_Sym *symtab = (Elf32_Sym *)(base + sym_sh->sh_offset);//符号表
  int sym_cnt = sym_sh->sh_size / sizeof(Elf32_Sym);//符号条目数量
  char *strtab = (char *)(base + sh[str_idx].sh_offset);//字符串表
  func_cnt = 0;//重置函数计数器
  for (int i = 0; i < sym_cnt && func_cnt < MAX_FUNCS; i++) {
    Elf32_Sym *s = &symtab[i];//当前符号
    if (ELF32_ST_TYPE(s->st_info) != STT_FUNC) {//只收集函数符号
      continue;//只收集函数符号
    }
    if (s->st_value == 0 || s->st_name == 0) {
      continue;
    }
    const char *name = strtab + s->st_name;
    if (!name || name[0] == '\0') {
      continue;
    }
    FuncSym *f = &funcs[func_cnt++];//添加函数符号到表
    f->start = s->st_value;//函数入口地址
    f->end = (s->st_size == 0) ? s->st_value : (s->st_value + s->st_size);
    strncpy(f->name, name, sizeof(f->name)-1);//复制函数名，确保字符串结尾
    f->name[sizeof(f->name)-1] = '\0';
  }
  printf("ftrace: loaded %d functions from %s\n", func_cnt, path);
  munmap(buf, size);
  close(fd);
}

static const FuncSym *find_by_entry(uint32_t addr) {//先按函数入口地址精确匹配
  for (int i = 0; i < func_cnt; i++) {
    if (funcs[i].start == addr) {
      return &funcs[i];//按入口地址匹配
    }
  }
  return NULL;
}
static const FuncSym *find_by_pc(uint32_t addr) {//再按地址区间匹配，兼容符号不完整场景
//按地址落在[start,end)区间匹配(ret时按当前pc反查)
  for (int i = 0; i < func_cnt; i++) {
    if (addr >= funcs[i].start && addr < funcs[i].end) {
      return &funcs[i];//按地址区间匹配
    }
  }
  return NULL;
}

//初始化ftrace符号表与计数器
void ftrace_init(NPCContext* ctx) {
  if (!ctx) {
    return;
  }
  load_symbols(ctx->elf_path);//加载函数符号
  ftrace_line_count = 0;//重置输出行数
}

//记录一次call/ret轨迹
void ftrace_log(uint64_t pc, uint64_t target_pc, int is_call) {
#ifdef FTRACE_MAX_LINES
  if (ftrace_line_count >= FTRACE_MAX_LINES) {//限制输出行数，避免过多日志
    return;//限制输出行数
  }
#endif

  uint32_t pc32  = (uint32_t)pc;//当前指令PC
  uint32_t tgt32 = (uint32_t)target_pc;//目标地址(call目标或ret目标)
  if (is_call) {
    const FuncSym *f = find_by_entry(tgt32);//先按入口查找
    if (!f) {
      f = find_by_pc(tgt32);//再按区间查找
    }
    //先入口后区间: call目标通常是函数入口,区间匹配用于兼容符号不完整场景
    if (f) {
      printf("[ftrace] call %s @ 0x%08x <- 0x%08x\n", f->name, f->start, pc32);//输出函数调用
    } else {
      printf("[ftrace] call 0x%08x <- 0x%08x\n", tgt32, pc32);//输出地址调用
    }
  } else {
    const FuncSym *f = find_by_pc(pc32);//返回时按PC反查，因为ret指令没有明确目标地址，无法按入口匹配
    //因为ret路径没有显式的函数入口信息，所以用当前pc落区间反推所属函数
    if (f) {//如果找到所属函数
      printf("[ftrace] ret  %s (@0x%08x)\n", f->name, pc32);//输出函数返回
    } else {
      printf("[ftrace] ret  @0x%08x\n", pc32);//输出地址返回
    }
  }
  ftrace_line_count++;//输出后递增
}
