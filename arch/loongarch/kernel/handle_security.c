
#include <asm/loongarch.h>
#include <asm/handle_security.h>
#include <asm/shadow_stack.h>
#include <linux/sched.h>

unsigned long stack_buffer[MAX_SHADOW_STACK_SIZE * MAX_SHADOW_STACK_BACKUPS];
unsigned long top = 0;

#ifdef CONFIG_SHADOW_STACK
asmlinkage void context_switch_ss(struct task_struct *prev, struct task_struct *next, unsigned long ret_ptr)
{
    unsigned long save_a0 = 0;
    unsigned long save_a1 = 0;
    unsigned long save_tp = 0;
	asm volatile("move %0, $a0" : "=r"(save_a0));
	asm volatile("move %0, $a1" : "=r"(save_a1));
	asm volatile("move %0, $tp" : "=r"(save_tp));

    prev->thread.shadow_stack_top = top;
    memcpy(prev->thread.shadow_stack_buffer, stack_buffer, top/8 * sizeof(unsigned long));
    unsigned long live_top = r_scfg_d(0x28);
    for(int i = live_top - 8 ; i >= 0 ; i -= 8){
        prev->thread.shadow_stack_buffer[(prev->thread.shadow_stack_top + i)/8] = r_scfg_d(SHADOW_STACK_BASE - i);
        safe_pop();
    }
    prev->thread.shadow_stack_top += live_top;

    unsigned long left = next->thread.shadow_stack_top % MAX_SHADOW_STACK_SIZE;
    top = next->thread.shadow_stack_top - left;
    memcpy(stack_buffer, next->thread.shadow_stack_buffer, top/8 * sizeof(unsigned long));
    for(int i = 0 ; i < left ; i += 8){
        safe_push(next->thread.shadow_stack_buffer[(top + i)/8]);
    }

	asm volatile("move $a0, %0" : :"r"(save_a0));
	asm volatile("move $a1, %0" : :"r"(save_a1));
	asm volatile("move $tp, %0" : :"r"(save_tp));
	asm volatile("jirl $r0, %0, 0" : :"r"(ret_ptr));
}

void handle_ss_overflow(void)
{
    unsigned long size = r_scfg_w(8);
    for(int i = size-8 ; i >= 0 ; i-=8){
        stack_buffer[(top + i)/8] = r_scfg_d(SHADOW_STACK_BASE - i);
        safe_pop();
    }
    top += size;
    asm volatile("b exit_handle_sec");
}

void handle_ss_underflow(void)
{
    unsigned long size = r_scfg_w(8);
    top -= size;
    for(int i = 0 ; i < size ; i+=8){
        safe_push(stack_buffer[(top + i)/8]);
    }
    asm volatile("b exit_handle_sec");
}

void handle_ss_illegal(void)
{
    asm volatile("b exit_handle_sec");
}
#endif