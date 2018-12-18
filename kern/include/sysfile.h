#include <synch.h>
#include <vnode.h>
#include <mips/trapframe.h>

struct sysfile {
    struct vnode *file_node;
    int64_t seek_addr;
    int flags;
    int duppedFD;
};