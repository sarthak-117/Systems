#include "xmalloc.h"
#include "hmalloc.h"
#include "hmalloc.c"

void* 
xmalloc(size_t size) {
	return hmalloc(size);
}

void 
xfree(void* ptr) {
	hfree(ptr);
}

void* 
xrealloc(void* ptr, size_t bytes) {
	return hrealloc(ptr, bytes);
}
