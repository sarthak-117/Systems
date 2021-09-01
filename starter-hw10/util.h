// based on cs3650 starter code

#ifndef UTIL_H
#define UTIL_H

#include <string.h>
#include "slist.h"
#include "directory.h"

static int
streq(const char* aa, const char* bb)
{
    return strcmp(aa, bb) == 0;
}

static int
min(int x, int y)
{
    return (x < y) ? x : y;
}

static int
max(int x, int y)
{
    return (x > y) ? x : y;
}

static int
clamp(int x, int v0, int v1)
{
    return max(v0, min(x, v1));
}

static int
bytes_to_pages(int bytes)
{
    int quo = bytes / 4096;
    int rem = bytes % 4096;
    if (rem == 0) {
        return quo;
    }
    else {
        return quo + 1;
    }
}

static void
join_to_path(char* buf, char* item)
{
    int nn = strlen(buf);
    if (buf[nn - 1] != '/') {
        strcat(buf, "/");
    }
    strcat(buf, item);
}

static void
get_parent_path(const char* path, char parent_path[]) {
/*	slist* path_split = directory_list(path);
	slist* free_list = path_split;
	if (path_split == NULL) {
		return;
	}

	while (path_split->next) {
		join_to_path(parent_path, path_split->data);
	}

	s_free(free_list);
	return;*/
	  memset(parent_path, 0, 48);
    int nn = strlen(path);

    for (int ii = nn - 1; ii >= 0; --ii) {
        if (path[ii] == '/' && ii != 0) {
            memcpy(parent_path, path, ii);
            parent_path[ii] = '\0';
            return;
        }
        if (ii == 0) {
            memcpy(parent_path, path, 1);
            parent_path[1] = '\0';
        }
    }
}

static void
get_name(const char* path, char name []) {
/*	slist* path_split = directory_list(path);
	slist* free_list = path_split;
	while(path_split) {
		if (path_split->next == NULL) {
			strcpy(name, path_split->data);
		}
		path_split = path_split->next;
	}
	s_free(free_list);
	return;*/
	 memset(name, 0, 48);
    int nn = strlen(path);

    for (int ii = nn - 1; ii >= 0; --ii) {
        if (path[ii] == '/') {
            memcpy(name, (path + ii + 1), (nn - ii - 1));
            name[nn - ii] = '\0';
            return;
        }
    }
}

#endif
