#ifndef ARGUMENTS_H
#define ARGUMENTS_H
#include "barrier.h"
#include "float_vec.h"

typedef struct arguments {
	int thread_number;
	float* data;
	long size;
	int P;
	floats* samps;
	long* sizes;
	barrier* bb;
	const char* output;	
} arguments;

arguments* make_arguments(int thread_number, float* data, long size, int P, floats* samps, long* sizes, barrier* bb, const char* output);
#endif

