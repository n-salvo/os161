#include <types.h>
#include <vnode.h>



//this file is not yet included in the build path
// keep track of files that are open


struct proc_file_table {
    struct sysfile *files[4096];
    //int count;
};

void proc_file_table_init(struct proc_file_table **table);

void proc_file_table_destroy(struct proc_file_table *table);

/*
 * Returns 1 if the vnode is successfully added
 * returns 0 if the table already contains the vnode
 */
int table_addnode(struct proc_file_table *table, struct vnode *node, int *descriptor, int mode);

/*
 * Returns NULL if the file descriptor is invalid
 */
struct vnode *table_getvnode(struct proc_file_table *table, int fileHandle);
struct sysfile *table_getfile(struct proc_file_table *table, int fileHandle);

int table_removenode(struct proc_file_table *table, int fileHandle);

int table_get_free_descriptor(struct proc_file_table *table);

void sysfile_shallow_copy(struct sysfile *from, struct sysfile *to);
void sysfile_deep_copy(struct sysfile *from, struct sysfile *to);

void table_deep_copy(struct proc_file_table *from, struct proc_file_table *to);
/*
 * Close the specified handle.
 * If the handle is a dup of an actual file, the dup is removed.
 * If the handle has others dups that reference it, the data at
 * the handle is cut and pasted to one of the other dups. All
 * dups that referenced the original handle will get updated
 * to reference the new handle.
 */
int table_close_fd(struct proc_file_table *table, int filehandle);

int table_dup2(struct proc_file_table *table, int filehandle, int newhandle);
/*
 * Returns 1 if the vnode is successfully removed
 * returns 0 if the table does not contain the vnode
 */
//int table_removenode(struct proc_file_table *table, struct vnode *node);

//struct sysfile *_get_sysfile(struct proc_file_table *table, struct vnode *node);