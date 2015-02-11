#ifndef PROCESS_H
# define PROCESS_H

# define ENABLE_INTERRUPTS() \
    asm volatile("cpsie i");

# define DISABLE_INTERRUPTS() \
    asm volatile("cpsid i");

# define RUN    0
# define WAIT   1
# define ZOMBIE 2
# define TERM   3
# define SLEEP  4   // We'll be used later!

# include <stdint.h>
# include "mem.h"
# include "timers.h"
# include "fs/vfs.h"

# define NBMAX_PROC 4096
# define NBMAX_FD   4096
# define STACK_SIZE 2048

typedef struct proc
{
    const char *name;
    int pid;
    int ppid;
    int status;
    int nbrun;
    int stack_idx;
    uint32_t pc;
    uint32_t sp;
    s_fd fd_table[NBMAX_FD];
    struct proc *next;
    struct proc *prev;
} s_proc;

s_proc *current_process;
int nbproc;
int real_nbproc;

void init_process(void);
int add_process(const char *name, uint32_t pc, int status/* stdio later... */);
int remove_process(int pid);

int kill(int pid, int status);
void exit(int status);

//TODO:
int fork_exec(const char *path, char **args); //returns pid
int fork_call(uint32_t addr, char **args); //returns pid
int waitpid(int pid); //returns the return value of the executed app on fork.

#endif /* !PROCESS_H */
