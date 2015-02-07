#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "thread_pool.h"

#define TRUE 1
void* worker_function( void *tp )
{
	tp_job *job;
	thread_pool *t_pool;

	/* Cast parameter to thread pool */
	t_pool = (thread_pool*) tp;

	/* Continously iterate over available requests */
	while (TRUE) {

		/* Block on available request */
		job = tp_dequeue_request( t_pool );

		/* work */
		fprintf(stderr, "THREAD: Incoming request from\n");

		/* Cleanup */
		tp_job_destroy(job);
	}

}
