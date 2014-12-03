#ifndef PROCESS_H
# define PROCESS_H

# include "io.h"

# define RUN    0
# define WAIT   1
# define ZOMBIE 2
# define TERM   3
# define ABORT  4

typedef struct context
{
    uint64_t sp;
    uint64_t pc;
} s_context;

typedef struct proc
{
    const char *name;
    int pid;
    int ppid;
    int status;
    s_context context;
    s_fd fd_table[NBMAX_FD];
    struct proc *next;
    struct proc *prev;
} s_proc;

extern s_proc *current_process;

s_proc *create_process(const char *name, s_context context /* stdio later... */);
int remove_process(s_proc *proc);
int run_process(s_proc *proc);

#endif /* !PROCESS_H */
