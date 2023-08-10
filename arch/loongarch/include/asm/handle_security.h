#ifndef HANDLE_SECURITY_H_
#define HANDLE_SECURITY_H_

#include <asm/shadow_stack.h>

extern unsigned long stack_buffer[MAX_SHADOW_STACK_SIZE * MAX_SHADOW_STACK_BACKUPS];
extern unsigned long top;

struct task_struct;

extern asmlinkage void context_switch_ss(struct task_struct *prev, struct task_struct *next, unsigned long ret_ptr);

extern void handle_ss_overflow(void);
extern void handle_ss_underflow(void);
extern void handle_ss_illegal(void);

#endif