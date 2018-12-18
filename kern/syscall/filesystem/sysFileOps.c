

#include <types.h>
#include <syscall.h>
//#include <lib.h>
#include <vfs.h>
#include <proc.h>
#include <current.h>


int sys_open(const char *filename, int flags) {    
    kprintf("attempting to open: %s\n", filename);
    struct vnode *fileNode = NULL;
    struct proc_file_table *table;
    char *filenamebuf = kstrdup(filename);
    int ret;
    int fd = -1;
    
    table = curproc->file_table;
    KASSERT(table != NULL);
    
    /* returns negative value on failure, 0 on success */
    ret = vfs_open(filenamebuf, flags, 0, &fileNode);
    if(ret < 0) {
        kprintf("open failed\n");
        return -1;
    }
    ret = table_addnode(table, fileNode, &fd, flags);
    if(ret < 0) {
        kprintf("open failed\n");
        vfs_close(fileNode);
        return -1;
    }
    kprintf("opened file descriptor: %d\n", fd);
    return fd;
}

int sys_close(int filehandle) {
    // struct vnode *fileNode;
    struct proc_file_table *table;
    
    table = curproc->file_table;
    KASSERT(table != NULL);

    // fileNode = table_getvnode(table, filehandle);
    // if(fileNode == NULL) {
    //     return -1;
    // }

    // vfs_close(fileNode);
    // KASSERT(fileNode->vn_refcount >= 0);
    // return table_removenode(table, filehandle);
    return table_close_fd(table, filehandle);
}

int sys_dup2(int filehandle, int newhandle) {
    struct proc_file_table *table;    
    table = curproc->file_table;
    if(newhandle == filehandle) {
        return newhandle;
    }
    return table_dup2(table, filehandle, newhandle);
}