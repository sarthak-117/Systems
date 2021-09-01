// based on cs3650 starter code

#ifndef INODE_H
#define INODE_H

#include "pages.h"

typedef struct inode {
    int refs; // reference count
    int mode; // permission & type
    int size; // bytes
    int ptrs[2]; // direct pointers
    int iptr; // single indirect pointer
} inode;

void print_inode(inode* node);
inode* get_inode(int inum);
int alloc_inode(int mode);
void free_inode();
int grow_inode(inode* node, int size);
int shrink_inode(inode* node, int size);
int inode_get_pnum(inode* node, int fpn);

// returns 0 on success, -1 on failure
void write_file(inode* node, const char* buf, size_t size, off_t offset);
void read_file(inode* node, char* buf, size_t size, off_t offset);

#endif
