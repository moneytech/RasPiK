#include "io.h"

static s_fd *fd_table[NBMAX_FD];
static s_dir *dir_table[NBMAX_DIR];

static const char *pwd = "/";

void chdir(const char *path)
{
    pwd = path;
}

const char *get_pwd(void)
{
    return pwd;
}

static void free_dir(s_dir *dir)
{
    for (int i = 0; i < dir->nbentries; i++)
        kfree(dir->entries[i]);
    kfree(dir->entries);
    kfree(dir);
}

s_dir *opendir(const char *name)
{
    if (!strcmp(name, ""))
        return NULL;

    char *path = name[0] == '/' ? (char *)name : strcat(pwd, name);
    s_vfsdir *vfsdir = getdir(path);
    if (!vfsdir)
        return NULL;

    s_dir *dir = kmalloc(sizeof (s_dir));
    if (!dir)
        return NULL;

    int i;
    for (i = 0; dir_table[i] && i < NBMAX_DIR; i++) {}
    if (i >= NBMAX_DIR)
    {
        kfree(dir);
        return NULL;
    }

    dir->idx = i;
    dir->name = path;
    dir->nbentries = vfsdir->nbinodes;
    dir->entries = kmalloc(dir->nbentries * sizeof (s_direntry *));
    if (!dir->entries)
    {
        kfree(dir);
        return NULL;
    }

    for (i = 0; i < dir->nbentries; i++)
    {
        s_direntry *entry = kmalloc(sizeof (s_direntry));
        if (!entry)
        {
            free_dir(dir);
            return NULL;
        }
        entry->type = vfsdir->list[i]->type;
        entry->name = vfsdir->list[i]->name;
        dir->entries[i] = entry;
    }

    dir_table[dir->idx] = dir;
    return dir;
}

int mkdir(const char *name)
{
    if (!strcmp(name, "") || !strcmp(name, "/"))
        return 0;

    char *path = name[0] == '/' ? (char *)name : strcat(pwd, name);
    s_vfsinode *inode = kmalloc(sizeof (s_vfsinode));
    if (!inode)
        return 0;

    int i;
    inode->inumber = inumber++;
    inode->lock = 0;
    inode->type = DIR;
    for (i = strlen(path) - 1; path[i] != '/'; i--) {}
    inode->name = path + i + 1;

    s_vfsdir *dir = kmalloc(sizeof (s_vfsdir));
    if (!dir)
    {
        kfree(inode);
        return 0;
    }

    dir->name = inode->name;

    dir->nbinodes = 2;
    dir->list = kmalloc(dir->nbinodes * sizeof (s_vfsinode *));

    s_vfsinode *dot = kmalloc(sizeof (s_vfsinode));
    if (!dot)
    {
        free_vfsinode(inode);
        return 0;
    }
    s_vfsinode *doubledot = kmalloc(sizeof (s_vfsinode));
    if (!doubledot)
    {
        kfree(dot);
        free_vfsinode(inode);
        return 0;
    }

    dot->inumber = inumber++;
    doubledot->inumber = inumber++;
    dot->lock = 0;
    doubledot->lock = 0;
    dot->type = DIR;
    doubledot->type = DIR;
    dot->name = ".";
    doubledot->name = "..";
    dot->node = (void *)dir;

    char *parent = kmalloc(strlen(path));
    for (int k = 0; k < i; k++)
        parent[k] = path[k];
    doubledot->node = getdir(parent);

    dir->list[0] = dot;
    dir->list[1] = doubledot;

    inode->node = (void *)dir;

    return add_vfsentry(path, inode);
}

void closedir(s_dir *directory)
{
    dir_table[directory->idx] = NULL;
    for (int i = 0; i < directory->nbentries; i++)
        kfree(directory->entries[i]);
    kfree(directory->entries);
    kfree(directory);
}

void print_dir(const char *path)
{
    s_dir *dir = opendir(path);
    write_console(dir->name, strlen(dir->name), WHITE);
    write_console(":\n\t", 3, WHITE);
    for (int i = 0; i < dir->nbentries; i++)
    {
        if (dir->entries[i]->type == DIR)
            write_console(dir->entries[i]->name, strlen(dir->entries[i]->name), BLUE);
        else
            write_console(dir->entries[i]->name, strlen(dir->entries[i]->name), WHITE);
        write_console(" ", 1, WHITE);
    }
    write_console("\n\n", 2, WHITE);
    closedir(dir);
}

static s_vfsinode *create_file(const char *path)
{
    s_vfsinode *inode = kmalloc(sizeof (s_vfsinode));
    if (!inode)
        return NULL;
    inode->inumber = inumber++;
    inode->lock = 0;
    inode->type = FILE;
    int i;
    for (i = strlen(path) - 1; path[i] != '/'; i--) {}
    inode->name = path + i + 1;

    s_vfsfile *file = kmalloc(sizeof (s_vfsfile));
    if (!file)
    {
        kfree(inode);
        return NULL;
    }
    file->name = inode->name;
    file->size = 0;
    file->data = kmalloc(sizeof (char));
    if (!file->data)
    {
        kfree(inode);
        kfree(file);
        return NULL;
    }
    file->data[0] = EOF;

    inode->node = (void *)file;
    int result = add_vfsentry(path, inode);
    if (!result)
    {
        free_vfsinode(inode);
        return NULL;
    }
    return inode;
}

int open(const char *name, int mode)
{
    char *path = name[0] == '/' ? (char *)name : strcat(pwd, name);
    s_fd *fd = kmalloc (sizeof (s_fd));
    if (!fd)
        return -1;
    fd->inode = get_vfsinode(path);
    if (!fd->inode)
    {
        if ((mode & O_CREAT) == O_CREAT)
            fd->inode = create_file(path);
        if (!fd->inode)
        {
            kfree(fd);
            return -1;
        }
    }
    if ((fd->inode->type != FILE) && (fd->inode->type != DEV))
    {
        kfree(fd);
        return -1;
    }
    fd->offset = 0;
    if (((mode & O_APPEND) == O_APPEND) && fd->inode->type == FILE)
        fd->offset = ((s_vfsfile *)fd->inode->node)->size;
    fd->flags = mode;
    if ((mode & O_RDWR) == O_RDWR)
    {
        if (fd->inode->lock == 0)
            fd->inode->lock = 1;
        else
        {
            if ((mode & O_CREAT) == O_CREAT)
                free_vfsinode(fd->inode);
            kfree(fd);
            return -1;
        }
    }
    int i;
    for (i = 0; fd_table[i] && i < NBMAX_FD; i++) {}
    if (i >= NBMAX_FD)
    {
        kfree(fd);
        return -1;
    }
    fd_table[i] = fd;
    return i;
}

int init_io(void)
{
    for (int i = 0; i < NBMAX_FD; i++)
        fd_table[i] = NULL;
    for (int i = 0; i < NBMAX_DIR; i++)
        dir_table[i] = NULL;

    //TESTS:
    if (!mkdir("/dev") || !mkdir("/home") || !mkdir("/etc"))
        return 0;
    if (!mkdir("/home/pi") || !mkdir("/home/root"))
        return 0;
    if (!mkdir("/home/pi/bla"))
        return 0;
    if (open("/home/pi/test", O_CREAT) == -1)
        return 0;
    if (open("/home/pi/bla/test2", O_CREAT | O_RDWR) == -1)
        return 0;
    //////////////////////////////////////////////////////////

    return 1;
}
