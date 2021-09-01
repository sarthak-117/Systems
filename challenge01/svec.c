/* This file is lecture notes from CS 3650, Fall 2018 */
/* Author: Nat Tuck */

#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include "svec.h"

svec*
make_svec()
{
    svec* sv = malloc(sizeof(svec));
    sv->data = malloc(2 * sizeof(char*));
    sv->size = 0;
    // TODO: correctly allocate and initialize data structure
    sv->capacity = 2;
    return sv;
}

void
free_svec(svec* sv)
{
    // TODO: free all allocated data
    for (int i = 0; i < sv->size; ++i) {
	    free(sv->data[i]);
    }
    free(sv->data);
    free(sv);
}

char*
svec_get(svec* sv, int ii)
{
    assert(ii >= 0 && ii < sv->size);
    return sv->data[ii];
}

void
svec_put(svec* sv, int ii, char* item)
{
    assert(ii >= 0 && ii < sv->size);
    sv->data[ii] = strdup(item);
    // TODO: insert item into slot ii
    // Consider ownership of string in collection.
    
}

void
svec_push_back(svec* sv, char* item)
{
    int ii = sv->size;

    // TODO: expand vector if backing erray
    // is not big enough
    if (ii == sv->capacity) {
        sv->data = realloc(sv->data, 2 * sv->capacity*sizeof(char*));
	sv->capacity *= 2;
    }
    sv->size = ii + 1;
    svec_put(sv, ii, item);
}

void
svec_swap(svec* sv, int ii, int jj)
{
    // TODO: Swap the items in slots ii and jj
    char* temp = sv->data[ii];
    sv->data[ii] = sv->data[jj];
    sv->data[jj] = temp;
}

int 
svec_contains(svec* sv, char* item) {
	for (int i = 0; i < sv->size; i++) {
		if (strcmp(svec_get(sv, i), item) == 0) {
			return 0;
		}
	}
	return -1;
}

void 
print_svec(svec* sv) {
	for (int i = 0; i < sv->size; ++i) {
		puts(svec_get(sv, i));
	}
}
