#include <types.h>
#include <syscall.h>
#include <proc.h>
#include <mips/trapframe.h>
#include <current.h>
//#include <kern/types.h>
//#include <sys/types.h>


pid_t sys_fork(void) {
    struct proc *newproc = kmalloc(sizeof(struct proc));
    struct proc *currentProc = curproc;

    struct vnode *node = kmalloc(sizeof(struct vnode));

    newproc->p_name = kstrdup(curproc->p_name);
    newproc->p_lock = curproc->p_lock;//create and init new lock instead?
    newproc->p_numthreads = curproc->p_numthreads;
    
    return -1;
}