#include "inode.h"
#include "bitmap.h"
#include "util.h"

const int NUM_DIR = 2;

inode*
get_inode(int inum) 
{
	void* inodes = pages_get_page(1);
	return (inode*)(inodes + sizeof(inode) * inum);
}

// returns index of node on success, -1 otherwise
int alloc_inode(int mode) {
	void* node_bm = get_inode_bitmap();

	for (int i = 1; i < 256; ++i) {
		if (!bitmap_get(node_bm, i)) {
			bitmap_put(node_bm, i, 1);

			inode* new_node = get_inode(i);
			new_node->refs = 1;
			new_node->mode = mode;
			new_node->size = 0;
			new_node->ptrs[0] = alloc_page();
			new_node->iptr = 0;
			return i;
		}
	}
	return -1;
}

void free_inode() {

}

void write_file(inode* node, const char* buf, size_t size, off_t offset) {
	void* page = pages_get_page(node->ptrs[0]);
	memcpy(page, buf, size);
}

void read_file(inode* node, char* buf, size_t size, off_t offset) {
	void* page = pages_get_page(node->ptrs[0]);
	memcpy(buf, (char*)(page), size);
}

