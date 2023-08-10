#ifndef SHADOW_STACK_INST_H_
#define SHADOW_STACK_INST_H_

#define SHADOW_STACK_BASE (0xffff8)


.macro def_insn insn, val, addr
parse_v __insn, \insn 
parse_r __val, \val
parse_r __addr, \addr
.word __insn | (__addr & 0x1f) << 5 | __val
.endm

.macro scfgr_w val, addr
def_insn 0x06492000 \val, \addr
.endm

.macro scfgr_d val, addr
def_insn 0x06492400 \val, \addr
.endm

.macro scfgw_w val, addr
def_insn 0x06492800 \val, \addr
.endm

.macro scfgw_d val, addr
def_insn 0x06492c00 \val, \addr
.endm

#endif
