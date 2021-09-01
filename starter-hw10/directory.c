#include <string.h>
#include "pages.h"
#include "directory.h"
#include "util.h"
#include "slist.h"
#include "util.h"
#include "assert.h"

int directory_lookup(inode* dd, const char* name) {
	if (strcmp(name, "/") == 0) {
		return 0;
	}
	void* directory = pages_get_page(dd->ptrs[0]);

	for (int ii = 0; ii < dd->size; ii += sizeof(dirent)) {
		dirent* curr  = (dirent*)(directory + ii);

		if (streq(curr->name, name)) {
			return curr->inum;
		}
	}
	return -1;
}

// probs want to use tree_lookup
int directory_put(inode* dd, const char* name, int inum) {
	void* directory = pages_get_page(dd->ptrs[0]);

	if (dd->size == 4096) {
	//	return -1;
	}
	if (directory_lookup(dd, name) != -1) {
	//	return -1;
	}

	dirent* curr = (dirent*) (directory + dd->size);

	char* entry_name = curr->name;
	strcpy(entry_name, name);
	curr->inum = inum;
	dd->size += sizeof(dirent);
	return 0;
}

slist* directory_list(const char* path) {
/*	slist* path_list = s_split(path, '/');
	slist* dup = path_list;
	while (dup) {
		puts(dup->data);
		dup = dup->next;
	}
	return path_list;*/
	slist* list = s_cons(path, 0);

        for (int ii = strlen(path) - 1; ii >= 0; --ii) {
              if (ii != 0 && path[ii] == '/') {
                  char str[48];
                  memcpy(str, path, ii);
                  str[ii] = '\0';
                  list = s_cons(str, list);
        }
    }

    return list;
}


int tree_lookup(const char* path) {
	    assert(path[0] == '/');

    if (streq(path, "/")) {
        return 0;
    }

    path += 1;

    int dir = 0;
    slist* pathlist = s_split(path, '/');
    slist* tmp = pathlist;

    while(tmp) {
	inode* node = get_inode(dir);
	dir = directory_lookup(node, tmp->data);
	if (dir == -1) {
	    return -1;
	}
	tmp = tmp->next;
    }

    return dir;
}

int 
directory_rename(inode* node, const char* from, const char* to) {
	void* directory = pages_get_page(node->ptrs[0]);

	for (int ii = 0; ii < node->size; ii += sizeof(dirent)) {
        	dirent* entry = (dirent*)(directory + ii);
       		char* entry_name = entry->name;

      	  	if (streq(entry_name, from)) {
            		strcpy(entry_name, to);
            		return 0;
        	}
    	}
	return -1;
}


