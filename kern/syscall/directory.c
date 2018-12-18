#include <types.h>
#include <syscall.h>
#include <thread.h>
#include <current.h>
#include <uio.h>
#include <vfs.h>

// int sys_getcwd(char *buf, size_t buflen) {
//     //vnode *res = curthread->t_proc->p_cwd;
//     //struct vnode *curDirectoryNode = NULL;
//     struct uio curDirectoryUIO;
//     char *testBuffer;
//     size_t testBufferLen = 50;
//     testBuffer = kmalloc(sizeof(char) * testBufferLen);
//     struct iovec stuff;

//     uio_kinit(&stuff, &curDirectoryUIO, testBuffer, testBufferLen, 0, UIO_READ); 

//     //int rv = vfs_getcurdir(&curDirectoryNode);
//     //handle return value (rv)
//     // rv = VOP_READ(curDirectoryNode, curDirectoryUIO);
//     vfs_getcwd(&curDirectoryUIO);

//     (void)buf;
//     (void)buflen;

//     uiomove(testBuffer, testBufferLen, &curDirectoryUIO);

//     int i;
//     for(i = 0; i < 30; i++) {
//         kprintf("%c", testBuffer[i]);
//     }
//     kprintf("\n");
    
//     // rv = VOP_NAMEFILE()

//     return 0;
// }

int sys_getcwd(char *buf, size_t buflen) {
    
    struct uio curDirectoryUIO;
    struct iovec stuff;

    uio_kinit(&stuff, &curDirectoryUIO, buf, buflen, 0, UIO_READ);
    vfs_getcwd(&curDirectoryUIO);


    uiomove(buf, buflen, &curDirectoryUIO);
    return 0;
}

int sys_chdir(const char *path) {
    return vfs_chdir(kstrdup(path));
}