#ifndef MMIO_H
# define MMIO_H

# include <stdint.h>

// Write value to mmio register.
static inline void write_mmio(uint32_t reg, uint32_t val)
{
    uint32_t *addr = (uint32_t *)reg;
    asm volatile(
                     "str %[val], [%[reg]]"
                     :: [reg]"r"(addr), [val]"r"(val)
                    );
}

// Read value from mmio register.
static inline uint32_t read_mmio(uint32_t reg)
{
    uint32_t *addr = (uint32_t *)reg;
    uint32_t ret;
    asm volatile(
                     "ldr %[ret], [%[reg]]"
                     : [ret]"=r"(ret)
                     : [reg]"r"(addr)
                    );
    return ret;
}

static inline void sync_mem(void)
{
    asm ("mcr p15, 0, ip, c7, c5, 0");
    asm ("mcr p15, 0, ip, c7, c5, 6");
    asm ("mcr p15, 0, ip, c7, c10, 4");
    asm ("mcr p15, 0, ip, c7, c5, 4");
}

static inline void dmb(void)
{
    asm volatile ("mcr p15, #0, %[zero], c7, c10, #5" : : [zero] "r" (0));
}

static inline void dsb(void)
{
    asm volatile ("mcr p15, #0, %[zero], c7, c10, #4" : : [zero] "r" (0));
}

static inline void flush(void)
{
    asm volatile ("mcr p15, #0, %[zero], c7, c14, #0" : : [zero] "r" (0));
}

#endif /* !MMIO_H */
