#ifndef VFS_H
# define VFS_H

# include "mem.h"
# include "console.h"
# include "utils.h"
# include "driver.h"

/*
** We don't need more that this virtual file system for this bare-metal OS
** until we add external mass storage devices.
*/

/* Usefull defines. */

# define NBMAX_FD  4096
# define NBMAX_DIR 4096

# define DIR       0
# define FILE      1
# define EXEC      2
# define DEV       3

# define EOF       -1

/* Virtual file system structures */

typedef struct vfsinode
{
    int inumber;
    int type;
    const char *name;
    void *node;
} s_vfsinode;

typedef struct vfsfile
{
    const char *name;
    uint32_t size;
    char *data;
} s_vfsfile;

typedef struct vfsdev
{
    const char *name;
    void *addr;
    s_driver *drv;
} s_vfsdev;

typedef struct vfsdir
{
    const char *name;
    int nbinodes;
    s_vfsinode **list;
} s_vfsdir;

typedef struct vfsexec
{
    const char *name;
    int (*addr)(int, char **);
} s_vfsexec;

/* Global variables */
int inumber;
s_vfsinode vfsroot;

/* External functions */
int init_vfs(void);
int add_vfsentry(const char *path, s_vfsinode *inode);
int add_execentry(const char *path, int (*addr)(int, char **));
int add_deventry(s_vfsdev *dev);
int remove_vfsentry(const char *path);
void print_vfs(void); //Just for debugging!

#endif /* !VFS_H */
