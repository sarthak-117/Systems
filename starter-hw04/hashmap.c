
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

// This provides strlcpy
// See "man strlcpy"
#include <bsd/string.h>
#include <string.h>

#include "hashmap.h"


int
hash(char* key)
{
    long hh  = 0;
    for (int ii = 0; key[ii]; ++ii) {
	    hh = hh * 77 + key[ii];
    }
    return hh;
}

hashmap*
make_hashmap_presize(int nn)
{
    hashmap* hh = calloc(1, sizeof(hashmap));
    // Double check "man calloc" to see what that function does.
    hh->capacity = nn;
    hh->size = 0;
    hh->data = calloc(nn, sizeof(hashmap_pair));
    return hh;
}

hashmap*
make_hashmap()
{
    return make_hashmap_presize(4);
}

void
free_hashmap(hashmap* hh)
{
    free(hh->data);
    free(hh);
}

int
hashmap_has(hashmap* hh, char* kk)
{
    return hashmap_get(hh, kk) != -1;
}

int
hashmap_get(hashmap* hh, char* kk)
{
    // key kk.
    // Note: return -1 for key not found.
    long val = hash(kk);
   
    for (long ii = val & (hh->capacity - 1); ii < hh->capacity; ++ii) {
	if (hh->data[ii].used == false) {
		    return -1;
	    }
        if (strcmp(hh->data[ii].key, kk) == 0 && !hh->data[ii].tomb) {
		    return hh->data[ii].val;
	}
    }
    return -1; 
}

void hashmap_grow(hashmap* hh) {
	long nn = hh->capacity;
	hashmap_pair* pairs = hh->data;
	hh->capacity = 2 * nn;
	hh->data = calloc(hh->capacity, sizeof(hashmap_pair));
	hh->size = 0;

	for (long ii = 0; ii < nn; ++ii) {
		if (pairs[ii].used && !pairs[ii].tomb) {
		hashmap_put(hh, pairs[ii].key, pairs[ii].val);
		}
	}
	free(pairs);
}

void
hashmap_put(hashmap* hh, char* kk, int vv)
{
  //  printf("put called");
    // TODO: Insert the value 'vv' into the hashmap
    // for the key 'kk', replacing any existing value
    // for that key.
    if (hashmap_has(hh, kk) != 0) {
	int hash_index = hash(kk);
	for (int ii= hash_index & (hh->capacity-1); ii< hh->capacity; ++ii) {
		if (strcmp(hh->data[ii].key, kk) == 0) {
			hh->data[ii].val = vv;
			break;
		}
	}

    } else {
	double size = (double) hh->size;
	double cap = (double) hh->capacity;
	double load_factor = size/cap;
	if (load_factor >= 0.5) {
		hashmap_grow(hh);
	}
	int hash_index = hash(kk);
	for (int ii = hash_index & (hh->capacity-1); ii < hh->capacity; ++ii) {
		if (hh->data[ii].used == false) {
			strlcpy(hh->data[ii].key, kk, 4);
			hh->data[ii].val = vv;
			hh->data[ii].used = true;
			hh->data[ii].tomb = false;
			hh->size = hh->size + 1;
			break;
		}
	}
    }
}

void
hashmap_del(hashmap* hh, char* kk)
{
    // TODO: Remove any value associated with
    // this key in the map.
    int hash_index = hash(kk);
    for (int ii = hash_index & (hh->capacity-1); ii < hh->capacity; ii++) {
	    if (strcmp(hh->data[ii].key, kk) == 0) {
		    hh->data[ii].tomb = true;
	    }
    }
}

hashmap_pair
hashmap_get_pair(hashmap* hh, int ii)
{
    return hh->data[ii];
}

void
hashmap_dump(hashmap* hh)
{
    printf("== hashmap dump ==\n");
  
    for (int ii = 0 ; ii < hh->capacity; ++ii) {
	    if (hh->data[ii].used && !hh->data[ii].tomb) {
	    printf("key: %s, val: %d, size: %d \n", hh->data[ii].key, hh->data[ii].val, hh->size);
	    }
    }
}


