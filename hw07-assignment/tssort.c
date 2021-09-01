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
#include "arguments.h"
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
sort_worker(int pnum, float* data, long size, int P, floats* samps, long* sizes, barrier* bb, const char* output)
{
    floats* xs = make_floats(0);

    for (int i = 0; i < size; ++i) {
	    if (data[i] >= samps->data[pnum] && data[i] < samps->data[pnum + 1]) {
		    floats_push(xs, data[i]);
	    }
    }
   
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

   //printf("%d: start: %ld\n", pnum, start);

    int od = open(output, O_WRONLY); 
    check_rv(od);

    int offset = sizeof(long) + sizeof(float) * start;
    int rv = lseek(od, offset, SEEK_SET);
    check_rv(rv);
    rv = write(od, xs->data, xs->size * sizeof(float));
    check_rv(rv);
    //for (long i = 0; i < xs->size; ++i) {
	   // data[start+ i] = xs->data[i];
	  //  printf("%f\n", xs->data[i]);
//	   float fl = xs->data[i];
//	   rv =  write(od, &fl, 4);
//	   check_rv(rv);	   
  //  }

    //printf("copied into array in process %d\n", pnum);
    close(od);
    free_floats(xs);
}

void* 
producer_thread(void* arg) {
	arguments* args = (arguments*) arg;
	sort_worker(args->thread_number, args->data, args->size, args->P, args->samps, args->sizes, args->bb, args->output);
	free(args);
	return 0;
}

void
run_sort_workers(float* data, long size, int P, floats* samps, long* sizes, barrier* bb, const char* output)
{
    pthread_t threads[P];
    (void) threads; // suppress unused warning

    // TODO: spawn P processes, each running sort_worker
 //   arguments* args = malloc(sizeof(arguments));
 //   args->data = data;
 //   args->size = size;
 //   args->P = P;
 //   args->samps = samps;
 //   args->sizes = sizes;
 //   args->bb = bb;
  //  arguments** args_array[P];


    for (int ii = 0; ii < P; ++ii) {
   	arguments* args = malloc(sizeof(arguments));
    	args->data = data;
   	args->size = size;
   	args->P = P;
    	args->samps = samps;
   	args->sizes = sizes;
    	args->bb = bb;
    	args->thread_number = ii;
    	args->output = output;
   // args_array[ii] = &args;
    int rv = pthread_create(
			    &(threads[ii]), 0,
			    producer_thread, (void*) args);
    check_rv(rv);
    }

    for (int ii = 0; ii < P; ++ii) {
       	pthread_join(threads[ii], 0);
//	free(args_array[ii]);
    }

   // free(args);

}

void
sample_sort(float* data, long size, int P, long* sizes, barrier* bb, const char* output)
{
    floats* samps = sample(data, size, P);

   // floats_print(samps);
    run_sort_workers(data, size, P, samps, sizes, bb, output);
    free_floats(samps);
}

int
main(int argc, char* argv[])
{
    alarm(120);

    if (argc != 4) {
        printf("Usage:\n");
        printf("\t%s P data.dat output\n", argv[0]);
        return 1;
    }

    const int P = atoi(argv[1]);
    const char* fname = argv[2];
    const char* oname = argv[3];

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


    int od = open(oname, O_CREAT | O_WRONLY, 0644); //O_TRUNC
    check_rv(od);
    ftruncate(od, fsize);

    long count; 
    read(fd, &count, sizeof(long));
    write(od, &count, sizeof(long));
    floats* data = make_floats(count);
    rv = read(fd, data->data, count*sizeof(float));
    check_rv(rv);

    //for (int i = 0; i < count; ++i) {
//	    float val;
	//    read(fd, &val, 4);
	  //  floats_push(data, val);
   // }
    close(fd);
    close(od);
    

    //float* data = malloc(1024);

   // printf("count = %ld\n", count);

    long sizes_bytes = P * sizeof(long);
    long* sizes = malloc(sizes_bytes);

    barrier* bb = make_barrier(P);

    sample_sort(data->data, count, P, sizes, bb, oname);

    free_barrier(bb);
    free(sizes);
   // free_floats(data);

  //  for (int i = 0; i < count; ++i) {
//	    write(od, &data->data[i], 4);
 //   }
 //   close(od);
    free_floats(data);
    // TODO: munmap your mmaps

   // od = open(oname, O_RDWR); 
   // read(od, &count, 8); 
   // printf("count = %ld\n", count);

   // float dat;
    //for (int i = 0; i < count; i++) {
	//    read(od, &dat, 4);
	 //   printf("%f, ", dat);
   // }

    return 0;
}
