#include <types.h>
#include <syscall.h>
#include <kern/stat.h>
#include <proc.h>
#include <current.h>
#include <kern/fcntl.h>
#include <sysfile.h>
#include <uio.h>
#include <kern/seek.h>
#include <kern/errno.h>
#include <mips/trapframe.h>
//#include <sys/types.h>



int sys_read(int filehandle, void *buf, size_t buf_size) {
    struct proc_file_table *table;
    struct sysfile *file;
    int readable = O_RDONLY | O_RDWR;
    int result = 0;

    table = curproc->file_table;
    KASSERT(table != NULL);

    file = table_getfile(table, filehandle);
    if(file == NULL || !(file->flags & readable)) {//file not opened with any write flags
        return -1;
    }

    struct uio writeUIO;
    struct iovec thing;

    uio_kinit(&thing, &writeUIO, buf, buf_size, file->seek_addr, UIO_READ);
    result = VOP_READ(file->file_node, &writeUIO);
    file->seek_addr += buf_size;
    
    return result;
}

int sys_write(int filehandle, const char *buf, size_t buf_size) {
    struct proc_file_table *table;
    struct sysfile *file;
    int writable = O_WRONLY | O_RDWR;
    int result = 0;
    char *writebuf = NULL;
    writebuf = kstrdup(buf);

    table = curproc->file_table;
    KASSERT(table != NULL);

    file = table_getfile(table, filehandle);
    if(file == NULL || !(file->flags & writable)) {//file not opened with any write flags
        return -1;
    }

    struct uio writeUIO;
    struct iovec thing;

    uio_kinit(&thing, &writeUIO, writebuf, buf_size, file->seek_addr, UIO_WRITE);
    result = VOP_WRITE(file->file_node, &writeUIO);
    file->seek_addr += buf_size;
    
    kfree(writebuf);

    return result;
}

off_t sys_lseek(int filehandle, off_t pos, int code) {
    struct proc_file_table *table;
    struct sysfile *file;
    int64_t position = 0L;


    table = curproc->file_table;
    KASSERT(table != NULL);

    //fail if the filehanle is for a device that can't seek ex. console


    file = table_getfile(table, filehandle);
    if(file == NULL) {
        //errno = EBADF;
        return -1;
    }

    struct stat *file_stat = kmalloc(sizeof(struct stat));
    KASSERT(file_stat != NULL);

    VOP_STAT(file->file_node, file_stat);

    switch(code) {
        case SEEK_SET:
            position = pos;
            break;
        case SEEK_CUR:
            position = (file->seek_addr) + pos;
            break;
        case SEEK_END:
            position = file_stat->st_size + pos;

            break;
        default:
            return -1;
    }
    if(position < 0) {
        
        return -1;
    }
    file->seek_addr = position;
    return position;
}