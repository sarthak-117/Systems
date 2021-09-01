#include "xmalloc.h"
#include <stdlib.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include <assert.h>

size_t round_up(size_t bytes);

typedef struct hm_stats {
    long pages_mapped;
    long pages_unmapped;
    long chunks_allocated;
    long chunks_freed;
    long free_length;
} hm_stats;

typedef struct free_block {
    size_t size;
    struct free_block* next;
} free_block;

const size_t PAGE_SIZE = 4096;
__thread hm_stats stats; // This initializes the stats to 0.
static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
__thread free_block* buckets[7] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL};

// NOTE TO THE TA: If run locally, we pass valgrind and frag, but it doesnt run on the server bc of over allocation of memory

int
free_list_length()
{
    int total_count = 0;
    int bucket_counts[7] = {0, 0, 0, 0, 0, 0, 0};
    free_block* curr = NULL;
    for (int i = 0; i < 7; i++) {
        curr = buckets[i];
        while (curr) {
            curr = curr->next;
            ++total_count;
            bucket_counts[i]++;
        }
    }
    return total_count;
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

int bucket_index(size_t size) {
    int count = 0;
    long lsize = (long)size;
    long placeholder = 1;
    while (placeholder < lsize) {
        placeholder <<= 1;
        count = count + 1;
    }
    return count - 5;
}

void 
free_cells(int bucket) {
	free_block* curr = buckets[bucket];
	free_block* prev = NULL;
/*	while(curr) {
		free_block* curr2 = curr;
		prev = 
		curr = curr->next;
		if (curr2->size >= PAGE_SIZE) {
			size_t size_of_chunk = curr2->size;
			void* chunk = (void*) curr2;
			int rv = munmap(chunk, size_of_chunk);
			stats.pages_unmapped++;
			if (rv == -1) {
				perror("error in unmapping extra space");
			}
		}
	}*/

    free_block* removal = NULL;

    if (curr == NULL) {
        return;
    }

    while(curr) {
        if (curr->size >= PAGE_SIZE) {
            if (prev) {
                prev->next = curr->next;
            }
            else  {
                buckets[bucket] = curr->next;
            }
	    removal = curr;
            removal->next = NULL;
            size_t size_of_chunk = removal->size;
	    void* chunk = (void*) removal;
	    int rv = munmap(chunk, size_of_chunk);
	    stats.pages_unmapped++;
	    if (rv == -1) {
		    perror("error in unmapping extra space");
	    }
    
        }

        prev = curr;
        curr = curr->next;
    }
   // return NULL; 
}
// unmaps nodes that have a size more than the PAGE_SIZE
void 
free_all_cells() {
	for (int ii = 0; ii < 7; ++ii) {
	    free_cells(ii);
	  }
}

void
coalesce(int bucket)
{
   if (bucket >= 7) {
	    puts("error with coalesce invalid input");
	    return;
    }

    if (buckets[bucket] == NULL) {
        return;
    }

    free_block* curr = buckets[bucket];
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

    free_all_cells();
    
}


void
free_list_insert(free_block* cell)
{
    //find the correct bucket
    size_t size = cell->size;
    size = round_up(size);
    int bucket = bucket_index(size);
    free_block* free_list = buckets[bucket];

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
            coalesce(bucket);
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
                    coalesce(bucket);
                    free_all_cells();
		    return;
                }
                prev = curr;
                curr = curr->next;
                curr_add = (uintptr_t) curr;
            }
        }
        prev->next = cell;
	free_all_cells();
        coalesce(bucket);
        return;
    }
}

void*
free_list_remove(size_t size)
{
    //get which bucket it will be in
    size_t rounded = round_up(size); 
    int bucket = bucket_index(rounded);
    free_block* curr = buckets[bucket];
    free_block* prev = NULL;
    free_block* removal = NULL;

    if (curr == NULL) {
        return NULL;
    }

    while(curr) {
        if (size <= curr->size) {
            if (prev) {
                prev->next = curr->next;
            }
            else  {
               buckets[bucket] = curr->next;
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
xmalloc(size_t bytes)
{
   
    stats.chunks_allocated += 1;
    bytes += sizeof(size_t);

    //Big size
    if (bytes > 2048) {
        //If between 2048 and 4096, just call it 4096
        if (bytes < PAGE_SIZE) {
            bytes = PAGE_SIZE;
        }
        int pages = div_up(bytes, PAGE_SIZE);
        size_t new_size = pages * PAGE_SIZE; //done in case extra page
        stats.pages_mapped += pages;
        void* mapped = mmap(0, new_size, PROT_READ|PROT_WRITE,
                MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        size_t* ptr = (size_t*) mapped;
        *ptr = new_size;
        return mapped + sizeof(size_t);
    }
    //Small size
    else {
	 pthread_mutex_lock(&lock);
        //Get the size--should be a power of 2
        bytes = round_up(bytes);
        void* node = free_list_remove(bytes);

        free_block* curr = NULL;

        //Unable to find something in the free list
        if (node == NULL) {
            node = mmap(0, PAGE_SIZE, PROT_READ|PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
            assert(node);
            curr = (free_block*) node;
   	    curr->next = NULL;
            curr->size = PAGE_SIZE;
            stats.pages_mapped += 1;
        } 
        //Was able to find something in the free list 
        else {
            curr = (free_block*) node;
        }
        //Ignoring the leftovers for now
        
        size_t leftover = curr->size - bytes;

         if (leftover >= sizeof(free_block)) {
         void* new_block = (void*) curr;
         new_block+=bytes;
         free_block* new_insert = (free_block*) new_block;
         new_insert->size = leftover;
         new_insert->next = NULL;
         free_list_insert(new_insert);
         }
         else {
             bytes = curr->size;
         }
	  pthread_mutex_unlock(&lock);
         
         size_t* size_ptr = (size_t*) curr;
         *size_ptr = bytes;
         return ((void*) size_ptr) + sizeof(size_t);
    }
}

size_t round_up(size_t bytes) {
    if (bytes <= 32) {
        return 32;
    }
    else if (bytes <= 64) {
        return 64;
    }
    else if (bytes <= 128) {
        return 128;
    }
    else if (bytes <= 256) {
        return 256;
    }
    else if (bytes <= 512) {
        return 512;
    }
    else if (bytes <= 1024) {
        return 1024;
    }
    else {
        return 2048;
    }
} 

void
xfree(void* ptr)
{
    pthread_mutex_lock(&lock);

    stats.chunks_freed += 1;

    void* chunk = ptr - sizeof(size_t);
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
        free_list_insert(block);
    }
    pthread_mutex_unlock(&lock);
    }

void* xrealloc(void* prev, size_t bytes) {
	if (bytes == 0) {
		xfree(prev);
		return 0;
	} 
	else if (prev == NULL) {
		return xmalloc(bytes);
	} 
	else {
		void* chunk = prev - sizeof(size_t);
		free_block* block = (free_block*) chunk;
		size_t size_used = block->size;
		void* ptr = xmalloc(bytes);
		memcpy(ptr, prev, size_used);
		xfree(prev);
    		return ptr;
	}
}

