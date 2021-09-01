#include "bitmap.h"
#include <stdint.h>
#include <stdio.h>

int bitmap_get(void* bm, int ii) {
	int jj = ii / 8;
	uint8_t* map = (uint8_t*) bm;
	uint8_t map_index = map[jj];

	int result = (map_index >> (7 - (ii % 8))) & 1;
	return result;
}

void bitmap_put(void* bm, int ii, int vv) {
	if (vv == 0) {
		((uint8_t*)bm)[ii / 8] &= ~(1 << (ii & 7)); 
	} 
	else {
		((uint8_t*)bm)[ii / 8] |= 1 << (ii & 7);
	}
}

void bitmap_print(void* bm, int size) {
  for (int ii = 0; ii < size; ii++) {
        uint8_t b = ((uint8_t*)bm)[ii];

        for (int jj = 0; jj < 8; jj++) {
            printf("%d", !!((b << jj) & 0x80));
        }
        printf("\n");
    }
}
