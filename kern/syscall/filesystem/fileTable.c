#include <types.h>
#include <lib.h>
#include <fileTable.h>
#include <sysfile.h>
#include <kern/fcntl.h>
#include <vfs.h>
#include <kern/errno.h>

int table_len = 4096;

void proc_file_table_init(struct proc_file_table **table) {
    (*table) = kmalloc(sizeof(struct proc_file_table));
    int i;
    if ((*table) == NULL) {
		panic("Unable to allocate memory\n");
	}
    //(*table)->files = kmalloc(sizeof(struct sysfile)* 4096);
    // if ((*table)->files  == NULL) {
	// 	panic("Unable to allocate memory\n");
	// }
    // for(i = 0; i < table_len; i++) {
    //     (*table)->files[i] = NULL;
    // }
    for(i = 0; i < table_len; i++) {
        (*table)->files[i] = kmalloc(sizeof(struct sysfile));
        (*table)->files[i]->file_node = NULL;
        (*table)->files[i]->duppedFD = -1;
        (*table)->files[i]->seek_addr = 0L;
        (*table)->files[i]->flags = 0;
    }

    int ret = -1;
    int fd = -1;
    struct vnode *stdin = NULL, *stdout = NULL, *stderr = NULL;
    char *console = kstrdup("con:");
    ret = vfs_open(console, O_RDONLY, 0, &stdin);
    //KASSERT(ret >= 0);
    table_addnode((*table), stdin, &fd, O_RDONLY);
    
    console = kstrdup("con:");
    ret = vfs_open(console, O_WRONLY, 0, &stdout);
    //KASSERT(ret >= 0);
    table_addnode((*table), stdout, &fd, O_WRONLY);
    
    console = kstrdup("con:");
    ret = vfs_open(console, O_WRONLY, 0, &stderr);
    //KASSERT(ret >= 0);
    table_addnode((*table), stderr, &fd, O_WRONLY);

    kprintf("%d", ret);
    //kfree(console);
    // panic("Done initializing file table\n");
}

void proc_file_table_destroy(struct proc_file_table *table) {
    kfree(table->files);
    kfree(table);
}

/*
 * Returns descriptor if the vnode is successfully added
 * returns -1 if the table already contains the vnode
 */
int table_addnode(struct proc_file_table *table, struct vnode *node, int *fileHandle, int flags) {
    struct sysfile *file = kmalloc(sizeof(struct sysfile));
    int fd = table_get_free_descriptor(table);
    if(file == NULL || fd == -1) {
        (*fileHandle) = -1;
        return -1;
    }
    file->seek_addr = 0;
    file->file_node = node;
    file->flags = flags;
    file->duppedFD = -1;
    table->files[fd] = file;
    (*fileHandle) = fd;
    return fd;
}

/*
 * Returns the first free file descriptor in the table
 * If the table is full, -1 is returned
 */
int table_get_free_descriptor(struct proc_file_table *table) {
    int i;
    for(i = 0; i < table_len; i++) {
        if(table->files[i]->file_node == NULL && table->files[i]->duppedFD == -1) {
            return i;
        }
    }
    return -1;
}

int table_removenode(struct proc_file_table *table, int filehandle) {
    KASSERT(filehandle < table_len && filehandle > -1);
    kfree(table->files[filehandle]);
    table->files[filehandle] = NULL;
    return 0;
}

/*
 * Returns NULL if the file fileHandle is invalid
 */
struct vnode *table_getvnode(struct proc_file_table *table, int filehandle) {
    KASSERT(filehandle < table_len && filehandle > -1);
    // struct sysfile *file = table->files[fileHandle];
    struct sysfile *file = table_getfile(table, filehandle);
    return file->file_node;
}


/*
 * Returns NULL if the file fileHandle is invalid
 */
struct sysfile *table_getfile(struct proc_file_table *table, int filehandle) {
    KASSERT(filehandle < table_len && filehandle > -1);
    struct sysfile *file = table->files[filehandle];
    if(file->duppedFD != -1) {
        return table->files[file->duppedFD];
    }
    return file;
}

int table_close_fd(struct proc_file_table *table, int filehandle) {
    struct sysfile *file = table->files[filehandle];
    
    if(file->duppedFD != -1) {
        file->duppedFD = -1;
        return 0;
    }

    if(file->file_node == NULL) {
        return -1;
    }

    int i = 0;
    int newfilehandle = -1;
    //copy to the first fd referencing the old fd
    for(; i < table_len; i++) {
        if(i == filehandle) {
            continue;
        }
        if(table->files[i]->duppedFD == filehandle) {
            newfilehandle = i;
            sysfile_shallow_copy(file, table->files[newfilehandle]);
            i++;
            break;
        }
    }
    //continue searching for additional fds that reference the old fd.
    //Update them to reference the replacement fd
    for(; i < table_len; i++) {
        if(table->files[i]->duppedFD == filehandle) {
            table->files[i]->duppedFD = newfilehandle;
        }
    }
    
    if(newfilehandle == -1) {
        vfs_close(table->files[filehandle]->file_node);
        KASSERT(table->files[filehandle]->file_node->vn_refcount >= 0);
    }
    return 0;
}

void sysfile_shallow_copy(struct sysfile *from, struct sysfile *to) {
    to->file_node = from->file_node;
    to->seek_addr = from->seek_addr;
    to->flags = from->flags;
    to->duppedFD = from->duppedFD;
}

int table_dup2(struct proc_file_table *table, int filehandle, int newhandle) {
    struct sysfile *file = table->files[filehandle];
    table_close_fd(table, newhandle);
    if(file->file_node == NULL && file->duppedFD == -1) {
        return -EBADF;
    }
    if(file->duppedFD != -1) {
        table->files[newhandle]->duppedFD = file->duppedFD;
    } else {
        table->files[newhandle]->duppedFD = filehandle;
    }
    return 0;
}

void sysfile_deep_copy(struct sysfile *from, struct sysfile *to) {
    struct vnode *node = kmalloc(sizeof(struct vnode));
    struct spinlock *lock = kmalloc(sizeof(struct spinlock));
    spinlock_init(lock);
    node->vn_refcount = from->file_node->vn_refcount;
    node->vn_countlock = (*lock);
    node->vn_fs = from->file_node->vn_fs;
    node->vn_data = from->file_node->vn_data;
    node->vn_ops = from->file_node->vn_ops;
    to->file_node = node;
    to->seek_addr = from->seek_addr;
    to->flags = from->flags;
    to->duppedFD = from->duppedFD;
}

void table_deep_copy(struct proc_file_table *from, struct proc_file_table *to) {
    int i;
    for(i = 0; i < table_len; i++) {
        to->files[i] = kmalloc(sizeof(struct sysfile));
        sysfile_deep_copy(from->files[i], to->files[i]);
    }    
}