#ifndef SHADOW_STACK_H_
#define SHADOW_STACK_H_

#define SHADOW_STACK_BASE (0xffff8)
#define MAX_SHADOW_STACK_SIZE (0x800 / 8)
#define MAX_SHADOW_STACK_BACKUPS (0x7)

__asm__(".macro parse_v var val\n\t"
        "\\var = \\val\n\t"
        ".endm\n\t");

__asm__(".macro def_insn insn, val, addr\n\t"
        "parse_v __insn, \\insn \n\t"
        "parse_r __val, \\val\n\t"
        "parse_r __addr, \\addr\n\t"
        ".word __insn | (__addr & 0x1f) << 5 | __val\n\t"
        ".endm");

__asm__(".macro scfgr_w val, addr\n\t"
        "def_insn 0x06492000 \\val, \\addr \n\t"
        ".endm");

__asm__(".macro scfgr_d val, addr\n\t"
        "def_insn 0x06492400 \\val, \\addr \n\t"
        ".endm");

__asm__(".macro scfgw_w val, addr\n\t"
        "def_insn 0x06492800 \\val, \\addr \n\t"
        ".endm");

__asm__(".macro scfgw_d val, addr\n\t"
        "def_insn 0x06492c00 \\val, \\addr \n\t"
        ".endm");

static inline unsigned int r_scfg_w(unsigned int addr)
{
  unsigned int val;
  asm volatile("scfgr_w %0, %1" : "=r" (val) : "r"(addr));
  return val;
}

static inline unsigned long r_scfg_d(unsigned int addr)
{
  unsigned long val;
  asm volatile("scfgr_d %0, %1" : "=r" (val) : "r"(addr));
  return val;
}

static inline void w_scfg_w(unsigned int val, unsigned int addr)
{
  asm volatile("scfgw_w %0, %1" : :"r"(val), "r"(addr));
}

static inline void w_scfg_d(unsigned long val, unsigned int addr)
{
  asm volatile("scfgw_d %0, %1" : :"r"(val), "r"(addr));
}

static inline void safe_pop(void)
{
  asm volatile("slli.d $r0, $r0, 0x4");
}

static inline void safe_push(unsigned long val)
{
  unsigned long offset = r_scfg_d(0x28);
  w_scfg_d(val, SHADOW_STACK_BASE - offset);
  w_scfg_d(offset + 8, 0x28);
}

#endif
