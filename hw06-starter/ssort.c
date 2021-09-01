#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>

#include "float_vec.h"
#include "barrier.h"
#include "utils.h"


int cmpfunc (const void* a, const void* b) {
	if ((*(float*)a - *(float*)b) > 0) {
		return 1;
	} else if ((*(float*)a - *(float*)b) < 0) {
		return -1;
	} else {
		return 0;
	}
}

void
qsort_floats(floats* xs)
{
    // TODO: call qsort to sort the array
    // see "man 3 qsort" for details
    qsort(xs->data, xs->size, sizeof(float), cmpfunc);
}

floats*
sample(float* data, long size, int P)
{
    // TODO: sample the input data, per the algorithm decription
    floats* random_sets = make_floats(0);
    floats* indexes = make_floats(0);

    long n = 3*(P-1);

    for (int i = 0; i < n; ++i) {
	    int unique = 0;
	    int index = random() % size;
	    for (int j = 0; j < indexes->size; ++j) {
		    if (indexes->data[j] == index) {
			    unique = 1;
			    break;
		    }
	    }

	    if (unique == 0) {
		    floats_push(random_sets, data[index]);
		    floats_push(indexes, index);
	    } else {
		    i = i - 1;
	    }
    }

    qsort_floats(random_sets);
    floats* samples =  make_floats(0);
    floats_push(samples, 0);

    for (int i = 0; i < P -1 ; ++i) {
	    floats_push(samples, random_sets->data[(i*3) + 1]);
    }

    floats_push(samples, INFINITY);

    free_floats(random_sets);
    free_floats(indexes);

    return samples;
}

void
sort_worker(int pnum, float* data, long size, int P, floats* samps, long* sizes, barrier* bb)
{
    
    // TODO: select the floats to be sorted by this worker
    floats* xs = make_floats(0);

    for (int i = 0; i < size; ++i) {
	    if (data[i] >= samps->data[pnum] && data[i] < samps->data[pnum + 1]) {
		    floats_push(xs, data[i]);
	    }
    }
   
    //floats_print(xs);

    printf("%d: start %.04f, count %ld\n", pnum, samps->data[pnum], xs->size);
   
    qsort_floats(xs);  

  //  floats_print(xs);

   // puts("waiting");

    sizes[pnum] =  (long) xs->size;

    barrier_wait(bb);

    long start = 0;

    for (long i = 0; i < pnum; ++i) {
	    start += sizes[i];
    }

    long end = 0;
    for (long i = 0; i <= pnum; ++i) {
	    end += sizes[i];
    }

   // printf("%d: start: %ld end: %ld\n", pnum, start, end);

    for (long i = 0; i < xs->size; ++i) {
	    data[start+ i] = xs->data[i];
    }

    //printf("copied into array in process %d\n", pnum);

    free_floats(xs);
}

void
run_sort_workers(float* data, long size, int P, floats* samps, long* sizes, barrier* bb)
{
    pid_t kids[P];
    (void) kids; // suppress unused warning

    // TODO: spawn P processes, each running sort_worker
    for (int pp = 0; pp < P; ++pp) {
	    if ((kids[pp] = fork())) {
		    //do nothing
	    }
	    else {
		   sort_worker(pp, data, size, P, samps, sizes, bb);
		   exit(0);
	    }
    }

    for (int ii = 0; ii < P; ++ii) {
       	int rv = waitpid(kids[ii], 0, 0);
	check_rv(rv);
    }

}

void
sample_sort(float* data, long size, int P, long* sizes, barrier* bb)
{
    floats* samps = sample(data, size, P);
 //   floats_print(samps);
    run_sort_workers(data, size, P, samps, sizes, bb);
    free_floats(samps);
}

int
main(int argc, char* argv[])
{
    alarm(120);

    if (argc != 3) {
        printf("Usage:\n");
        printf("\t%s P data.dat\n", argv[0]);
        return 1;
    }

    const int P = atoi(argv[1]);
    const char* fname = argv[2];

    seed_rng();

    int rv;
    struct stat st;
    rv = stat(fname, &st);
    check_rv(rv);

    const int fsize = st.st_size;
    if (fsize < 8) {
        printf("File too small.\n");
        return 1;
    }

    int fd = open(fname, O_RDWR);
    check_rv(fd);
   

    void* file = mmap(
		    0,
		    fsize, // or 1024
		    PROT_READ|PROT_WRITE,
		    MAP_SHARED,
		    fd,    
		    0); // TODO: load the file with mmap.
    (void) file; // suppress unused warning.
    close(fd);

    // TODO: These should probably be from the input file.

    long* size_data = (long*) file;
    long count = size_data[0];
    float* data = (float*) file + 2;

 //   printf("count: %ld\n", count);
 //   printf("data: %f\n", data[0]);

    //for (int i = 0; i < count; ++i) {
//	    printf("%f, ", data[i]);
  //  }

 //   puts("");

    long sizes_bytes = P * sizeof(long);
    long* sizes = mmap(0,
		    sizes_bytes,
		    PROT_READ|PROT_WRITE,
		    MAP_SHARED|MAP_ANONYMOUS,
		    -1,    
		    0); // TODO: This should be shared

    barrier* bb = make_barrier(P); 

    sample_sort(data, count, P, sizes, bb);

  //  for (int i = 0; i < count; i++) {
//	    printf("%f, ", data[i]);
  //  }


    free_barrier(bb);

    // TODO: munmap your mmaps
    munmap(file, fsize);
    munmap(sizes, sizes_bytes);

    return 0;
}

