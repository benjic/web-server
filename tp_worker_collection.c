#include <stdlib.h>
#include <pthread.h>

#include "thread_pool.h"

tp_worker_collection *tp_wc_init(int max_workers)
{
	tp_worker_collection *tp_wc;
	
	tp_wc = malloc(sizeof(tp_worker_collection));

	tp_wc->num_workers = 0;
	tp_wc->max_workers = max_workers;
	tp_wc->workers = malloc(max_workers * sizeof(pthread_t*));

	return tp_wc;
}

void tp_wc_destroy(tp_worker_collection *tp_w)
{
	free(tp_w->workers);
	free(tp_w);
}

