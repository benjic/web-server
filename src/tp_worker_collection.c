#include <stdlib.h>
#include <pthread.h>

#include "thread_pool.h"

/*
 * Memory helper for allocating memory for worker collection 
 */
tp_worker_collection *tp_wc_init(int max_workers)
{
	tp_worker_collection *tp_wc;
	
	/* Create pointer to new struct */
	tp_wc = malloc(sizeof(tp_worker_collection));

	/* Field defaults */
	tp_wc->num_workers = 0;
	tp_wc->max_workers = max_workers;

	/* Create buffer of size max_workers */
	tp_wc->workers = malloc(max_workers * sizeof(pthread_t*));

	return tp_wc;
}

/*
 * Memory helper to cleanup worker collection structs
 */
void tp_wc_destroy(tp_worker_collection *tp_w)
{
	/* Free the buffer of workers and pointer */
	free(tp_w->workers);
	free(tp_w);
}

