// Author: Nat Tuck
// CS3650 starter code

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include <unistd.h>

#include "barrier.h"

barrier*
make_barrier(int nn)
{
    barrier* bb = malloc(sizeof(barrier));
    assert(bb != 0);
    pthread_mutex_init(&(bb->lock), NULL);
    pthread_cond_init(&(bb->condv), NULL);
//	int rv;
//	rv = sem_init(&(bb->barrier), 0, 0);
  //  if (rv == -1) {
//	    perror("at sem_init(barrier)");
//	    abort();
 //  }

   // rv = sem_init(&(bb->mutex), 0, 1);
//    if (rv == -1) {
//	    perror("at sem_init(barrier)");
//	    abort();
  //  }

    bb->count = nn;  // TODO: These can't be right.
    bb->seen  = 0;
    return bb;
}

void
barrier_wait(barrier* bb)
{
    int rv;
    rv = pthread_mutex_lock(&bb->lock);
    if (rv != 0) {
	    perror("lock(1)");
	    exit(1);
    }

    bb->seen += 1;

    if (bb->seen >= bb->count) {
        pthread_cond_broadcast(&bb->condv);
    } else {
	    while(bb->seen < bb->count) {
		    pthread_cond_wait(&bb->condv, &bb->lock);
	    }
    }
    pthread_mutex_unlock(&bb->lock);
}

void
free_barrier(barrier* bb)
{
   free(bb);
}

