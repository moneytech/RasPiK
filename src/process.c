#include "process.h"

static uint32_t stacks[NBMAX_PROC];

extern uint32_t get_sp(void);

int add_process(const char *name, uint32_t pc/* stdio later...*/)
{
    s_proc *process = kmalloc(sizeof (s_proc));
    if (!process)
        return -1;

    process->name = name;
    process->pid = nbproc;
    process->ppid = !current_process ? -1 : current_process->pid;
    process->status = WAIT;
    process->nbrun = 0;
    process->pc = pc;

    if (!stacks[0])
    {
        stacks[0] = get_sp();
        process->sp = stacks[0];
        process->stack_idx = 0;
    }
    else
    {
        int i;
        for (i = 0; i < NBMAX_PROC; i++)
        {
            if (!stacks[i])
            {
                stacks[i] = stacks[0] + (i * STACK_SIZE);
                process->sp = stacks[i];
                process->stack_idx = i;
                break;
            }
        }
        if (i >= NBMAX_PROC)
        {
            kfree(process);
            return -1;
        }
    }

    for (int i = 0; i < NBMAX_FD; i++)
        process->fd_table[i].inode = NULL;
    //STDIOS here!

    switch (nbproc)
    {
        case 0:
            current_process = process;
            break;
        case 1:
            current_process->next = process;
            process->next = current_process;
            current_process->prev = process;
            process->prev = current_process;
            break;
        default:
            current_process->next->prev = process;
            process->next = current_process->next;
            current_process->next = process;
            process->prev = current_process;
            break;
    }
    nbproc++;

    //Create corresponding file?

    return process->pid;
}

int remove_process(int pid)
{
    s_proc *process = current_process;
    if (!process)
        return -1;

    int base = process->pid;

    do
    {
        if (process->pid == pid)
        {
            stacks[process->stack_idx] = 0;
            process->prev->next = process->next;
            process->next->prev = process->prev;
            kfree(process);
            return process->pid;
        }
        process = process->next;
    } while (process->pid != base);

    //Kill children processes?

    return -1;
}

int kill(int pid, int status)
{
    s_proc *process = current_process;
    if (!process)
        return -1;

    int base = process->pid;

    do
    {
        if (process->pid == pid)
        {
            process->status = status;
            return process->status;
        }
        process = process->next;
    } while (process->pid != base);

    return -1;
}

void init_process(void)
{
    for (int i = 0; i < NBMAX_PROC; i++)
        stacks[i] = 0;
    current_process = NULL;
    nbproc = 0;
}