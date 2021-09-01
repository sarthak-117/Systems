
#include <stdlib.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdint.h>
#include "hmalloc.h"
#include <string.h>
#include <pthread.h>

typedef struct free_block {
	size_t size;
	struct free_block* next;
} free_block;

const size_t PAGE_SIZE = 4096;
static hm_stats stats; // This initializes the stats to 0.
free_block* free_list = NULL; // Initialize the free_list.
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;


long
free_list_length()
{
    // TODO: Calculate the length of the free list.
	int length = 0;
	free_block* curr = free_list;
	while (curr) {
		curr = curr->next;
		++length;
	}

    return length;
}

hm_stats*
hgetstats()
{
    stats.free_length = free_list_length();
    return &stats;
}

void
hprintstats()
{
    stats.free_length = free_list_length();
    fprintf(stderr, "\n== husky malloc stats ==\n");
    fprintf(stderr, "Mapped:   %ld\n", stats.pages_mapped);
    fprintf(stderr, "Unmapped: %ld\n", stats.pages_unmapped);
    fprintf(stderr, "Allocs:   %ld\n", stats.chunks_allocated);
    fprintf(stderr, "Frees:    %ld\n", stats.chunks_freed);
    fprintf(stderr, "Freelen:  %ld\n", stats.free_length);
}

static
size_t
div_up(size_t xx, size_t yy)
{
    // This is useful to calculate # of pages
    // for large allocations.
    size_t zz = xx / yy;

    if (zz * yy == xx) {
        return zz;
    }
    else {
        return zz + 1;
    }
}

void
coalesce()
{
	if (free_list == NULL) {
		return;
	}
	free_block* curr = free_list;
	while (curr->next) {
		uintptr_t curr_pointer = (uintptr_t) curr;
		uintptr_t next_pointer = (uintptr_t) curr->next;
		if (curr_pointer + curr->size == next_pointer) {
			curr->size += curr->next->size;
			curr->next = curr->next->next;
		}
		else {
			curr = curr->next;
		}
	}
}

void
free_list_insert(free_block* cell)
{
	if (free_list == NULL) {
		free_list = cell;
	}
	else {
		free_block* curr = free_list;
		free_block* prev = NULL;
		uintptr_t curr_add = (uintptr_t) curr;
		uintptr_t cell_add = (uintptr_t) cell;
		if (cell_add < curr_add) {
			cell->next = free_list;
			free_list = cell;

			coalesce();
			return;
		}
		else {
			prev = curr;
			curr = curr->next;
			curr_add = (uintptr_t) curr;
			while (curr) {
				if (cell_add < curr_add) {
					prev->next = cell;
					cell->next = curr;

					coalesce();
					return;
				}
				prev = curr;
				curr  = curr->next;
				curr_add = (uintptr_t) curr;
			}
		}
		prev->next = cell;
		coalesce();
		return;
	}
}

void* 
free_list_remove(size_t size) 
{
	free_block* curr = free_list;	
	free_block* prev = NULL;
	free_block* removal = NULL;

	if (free_list == NULL) {
		return NULL;
	}

	while(curr) {
		if (size <= curr->size) {
			if (prev) {
				prev->next = curr->next;
			}
		       	else  {
				free_list = curr->next;
			}
			
			curr->next = NULL;
			removal = curr;
			return (void*) removal;
		} 

		prev = curr;
		curr = curr->next;
	}


	return NULL; // reach here if we need to make a new node
}

void*
hmalloc(size_t size)
{
    pthread_mutex_lock(&lock);

    stats.chunks_allocated += 1;
    size += sizeof(size_t);
   
    
    if (size >= PAGE_SIZE) {
	    int pages = div_up(size, PAGE_SIZE);
	    size_t new_size = pages * PAGE_SIZE; //done in case extra page
	    stats.pages_mapped += pages;
	    void* mapped = mmap(0, new_size, PROT_READ|PROT_WRITE,
			    MAP_SHARED|MAP_ANONYMOUS, -1, 0);
	    size_t* ptr = (size_t*) mapped;
	    *ptr = new_size;
	     pthread_mutex_unlock(&lock);
	    return mapped + sizeof(size_t);	    	    	    
    } 
    else {
	        
	    void* node = free_list_remove(size); 
	   
	    //printf("%ld\n", free_list_length());
	    free_block* curr = NULL;

	    if (node == NULL) {
	   	 node = mmap(0, PAGE_SIZE, PROT_READ|PROT_WRITE,
	   	 MAP_SHARED|MAP_ANONYMOUS, -1, 0);
		 curr = (free_block*) node;
	   	 curr->next = NULL;
	   	 curr->size = PAGE_SIZE;
	   	 stats.pages_mapped += 1;
	    } else {
		    curr = (free_block*) node;
	    }
		size_t leftover = curr->size - size;

	     if (leftover >= sizeof(free_block)) {
		 void* new_block = (void*) curr;
	         new_block+=size;
		 free_block* new_insert = (free_block*) new_block;	 
	         new_insert->size = leftover;
		 new_insert->next = NULL;
	    	 free_list_insert(new_insert);
	     }
	     else {
		     size = curr->size;
	     } 
	     size_t* size_ptr = (size_t*) curr;
	     *size_ptr = size;
	     pthread_mutex_unlock(&lock);
	     return ((void*) size_ptr) + sizeof(size_t);
    }	     	
}

void
hfree(void* item)
{
    pthread_mutex_lock(&lock);

     stats.chunks_freed += 1;
    // TODO: Actually free the item.

    void* chunk = item - sizeof(size_t);
    free_block* block = (free_block*) chunk;
    size_t size = *(size_t*)chunk;
    int pages = div_up(size, PAGE_SIZE);
    if (size >= PAGE_SIZE) {
	   munmap(chunk, size);
	   stats.pages_unmapped += pages;
    } 
    else {
	    block->size = size;
	    block->next = NULL;
	    free_list_insert(block); //this causes me to fail more tests than i pass
	   // not really sure whats going on with my insert since it passes all other tests?
	   
    }
    pthread_mutex_unlock(&lock);
}

void* hrealloc(void* prev, size_t bytes) {
	if (bytes == 0) {
		hfree(prev);
		return 0;
	} 
	else if (prev == NULL) {
		return hmalloc(bytes);
	} 
	else {
    		void* mapped = hmalloc(bytes);
		void* chunk = prev - sizeof(size_t);
		free_block* block = (free_block*) chunk;
		size_t size_used = block->size;
    		memcpy(mapped, prev, size_used);
    		hfree(prev);
   		return mapped;
	}
}
