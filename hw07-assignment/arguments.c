#include "arguments.h"
#include "float_vec.h"
#include "barrier.h"
#include "stdlib.h"

arguments* make_arguments(int thread, float* data, long size, int P, floats* samps, long* sizes, barrier* bb, const char* output ) {
	arguments* ag = malloc(sizeof(arguments));
	ag->thread_number = thread;
	ag->data = data;
	ag->size = size;
	ag->P = P;
	ag->samps = samps;
	ag->sizes = sizes;
	ag->bb = bb;
	ag->output = output;
	return ag;
}
