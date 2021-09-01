#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"


list* 
cons(char* xx, list* xs) {
	list* ys = malloc(sizeof(list));
	ys->head = strdup(xx);
	ys->tail = xs;
	return ys;
}

void 
free_list(list* xs) {
	if (xs) {
		free_list(xs->tail);
		free(xs);
	}
}

void 
print_list(list* xs) {
	for (; xs; xs = xs->tail) {
		puts(xs->head);
	}
}

long 
length(list* xs) {
	long yy = 0;
	for (; xs; xs = xs->tail) {
		++yy;
	}
	return yy;
}

list*
reverse(list* xs) {
	list* ys = 0;
	for (; xs; xs = xs->tail) {
		ys = cons(xs->head, ys);
	}	
	return ys;
}

list*
rev_free(list* xs) {
	list* ys = reverse(xs);
	free_list(xs);
	return ys;
}

char** to_array(list* xs) {
    char** data = malloc(length(xs) * sizeof(char*));
    int i = 0;
    for (; xs; xs = xs->tail) {
        data[i] = xs->head;
        i++;
    }
    return data;
}

// returns 0 if the str is in it, -1 if it isn't
int contains(list* xs, char* str) {
	for (; xs; xs = xs->tail) {
		if (strcmp(xs->head, str) == 0) {
			return 0;
		}
	}
	return -1;
}


