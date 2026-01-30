#include "ftrace.h"
#include "common.h"

#if NPC_ENABLE_FTRACE

#include <vector>
#include <string>
#include <cstdio>
#include <cstdint>

#include <elf.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

struct FuncSym {
  uint32_t start;
  uint32_t end;
  std::string name;
};

static std::vector<FuncSym> funcs;



//限制输出总行数
static uint32_t ftrace_line_count = 0;



//从ELF符号表加载函数符号
static void load_symbols(const char *path) {
  if (!path || path[0] == '\0') return;

  int fd = open(path, O_RDONLY);
  if (fd < 0) return;

  struct stat st;
  if (fstat(fd, &st) < 0) { close(fd); return; }

  size_t size = st.st_size;
  void *buf = mmap(nullptr, size, PROT_READ, MAP_PRIVATE, fd, 0);
  if (buf == MAP_FAILED) { close(fd); return; }

  uint8_t *base = (uint8_t *)buf;
  Elf32_Ehdr *eh = (Elf32_Ehdr *)base;

  if (eh->e_ident[EI_MAG0] != ELFMAG0 ||
      eh->e_ident[EI_MAG1] != ELFMAG1 ||
      eh->e_ident[EI_MAG2] != ELFMAG2 ||
      eh->e_ident[EI_MAG3] != ELFMAG3) {
    munmap(buf, size);
    close(fd);
    return;
  }

  Elf32_Shdr *sh = (Elf32_Shdr *)(base + eh->e_shoff);
  int shnum = eh->e_shnum;

  Elf32_Shdr *sym_sh = nullptr;
  for (int i = 0; i < shnum; i++) {
    if (sh[i].sh_type == SHT_SYMTAB) { sym_sh = &sh[i]; break; }
  }
  if (!sym_sh) {
    for (int i = 0; i < shnum; i++) {
      if (sh[i].sh_type == SHT_DYNSYM) { sym_sh = &sh[i]; break; }
    }
  }
  if (!sym_sh) {
    munmap(buf, size);
    close(fd);
    return;
  }

  int str_idx = sym_sh->sh_link;
  if (str_idx < 0 || str_idx >= shnum) {
    munmap(buf, size);
    close(fd);
    return;
  }

  Elf32_Sym *symtab = (Elf32_Sym *)(base + sym_sh->sh_offset);
  int sym_cnt = sym_sh->sh_size / sizeof(Elf32_Sym);
  char *strtab = (char *)(base + sh[str_idx].sh_offset);

  funcs.clear();

  for (int i = 0; i < sym_cnt; i++) {
    Elf32_Sym &s = symtab[i];
    if (ELF32_ST_TYPE(s.st_info) != STT_FUNC) continue;
    if (s.st_value == 0 || s.st_name == 0) continue;
    const char *name = strtab + s.st_name;
    if (!name || name[0] == '\0') continue;

    FuncSym f;
    f.start = s.st_value;
    f.end   = (s.st_size == 0) ? s.st_value : (s.st_value + s.st_size);
    f.name  = name;
    funcs.push_back(f);
  }

  printf("ftrace: loaded %zu functions from %s\n",
         funcs.size(), path);

  munmap(buf, size);
  close(fd);
}

static const FuncSym *find_by_entry(uint32_t addr) {
  for (auto &f : funcs) {
    if (f.start == addr) return &f;
  }
  return nullptr;
}

static const FuncSym *find_by_pc(uint32_t addr) {
  for (auto &f : funcs) {
    if (addr >= f.start && addr < f.end) return &f;
  }
  return nullptr;
}



//初始化函数追踪（解析符号表）
void ftrace_init(NPCContext* ctx) {
  if (!ctx) return;
  load_symbols(ctx->elf_path);
  ftrace_line_count = 0;                                       //重置输出计数
}

// 被 npc_ftrace_log 调用，记录一次 CALL / RET
void ftrace_log(uint64_t pc, uint64_t target_pc, int is_call) {


  //超过最大行数后不再输出
#ifdef FTRACE_MAX_LINES
  if (ftrace_line_count >= FTRACE_MAX_LINES) return;
#endif

  uint32_t pc32  = (uint32_t)pc;
  uint32_t tgt32 = (uint32_t)target_pc;

  if (is_call) {
    const FuncSym *f = find_by_entry(tgt32);
    if (!f) f = find_by_pc(tgt32);

    if (f) {
      printf("[ftrace] call %s @ 0x%08x <- 0x%08x\n",
             f->name.c_str(), f->start, pc32);
    } else {
      printf("[ftrace] call 0x%08x <- 0x%08x\n", tgt32, pc32);
    }
  } else {
    const FuncSym *f = find_by_pc(pc32);
    if (f) {
      printf("[ftrace] ret  %s (@0x%08x)\n",
             f->name.c_str(), pc32);
    } else {
      printf("[ftrace] ret  @0x%08x\n", pc32);
    }
  }

  ftrace_line_count++;
}



//每步调用一次，用于检测CALL/RET指令
void ftrace_step(NPCContext* ctx) {
  (void)ctx;
}

#else  //NPC_ENABLE_FTRACE == 0

void ftrace_init(NPCContext* ctx)                 { (void)ctx; }
void ftrace_log(uint64_t pc, uint64_t target_pc, int is_call)
                                                 { (void)pc; (void)target_pc; (void)is_call; }
void ftrace_step(NPCContext* ctx)                 { (void)ctx; }

#endif
