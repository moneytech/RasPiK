#include "interrupts.h"

void treat_undef(void)
{
    klog("Bad Exception handled: UNDEF!\n", 30, RED);
    while (1) {}
}

void treat_unused(void)
{
    klog("Bad Exception handled: UNUSED!\n", 31, RED);
    while (1) {}
}

void treat_fiq(void)
{
    klog("Bad Exception handled: FIQ!\n", 28, RED);
    while (1) {}
}

void treat_swi(unsigned number, unsigned *reg)
{
    reg[0] = ((funcptr)syscall_table[number])(reg[0], reg[1], reg[2], reg[3]);
}

void treat_pref_abort(void)
{
    // DO STH HERE IF NECESSARY...
}

void treat_data_abort(void)
{
    uint32_t addr, fault_addr;

    asm volatile("mov %[addr], lr"
                 : [addr]"=r"(addr));
    asm volatile("mrc p15, 0, %[addr], c6, c0, 0"
                 : [addr]"=r"(fault_addr));

    klog("DATA ABORT!\n", 12, BLUE);
    while (1) {}
}

void treat_irq(void)
{
    timerarm->clr_irq = IRQ_TIMERARM;
    schedule();
}

static void vector(void)
{
    asm volatile(
                 "b treat_reset\n"
                 "b treat_undef\n"
                 "b swi_handler\n"
                 "b treat_pref_abort\n"
                 "b treat_data_abort\n"
                 "b treat_unused\n"
                 "b irq_handler\n"
                 "b treat_fiq\n"
                );
}

void init_interrupts(void)
{
    klog("[", 1, WHITE);
    klog("...", 3, RED);
    klog("]", 1, WHITE);

    interrupts = (s_interrupts *)BASE_INTERRUPTS;

    asm volatile ("mcr p15, 0, %[addr], c12, c0, 0"
                  :: [addr]"r"(&vector));
    asm volatile ("cpsie i");

    interrupts->irq_en0 = IRQ_TIMERARM;

    wait(HUMAN_TIME / 2);
    klog("\b\b\b\bOK", 6, GREEN);
    klog("]\tInterrupt vector initialized!\n", 32, WHITE);
}
