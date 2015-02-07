#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>

#include "thread_pool.h"
#define BUFFER_RATIO 5

thread_pool *tp_init(int num_workers)
{
	thread_pool *tp;

	tp = malloc(sizeof(thread_pool));
	tp->jobs = tp_job_queue_init(num_workers * BUFFER_RATIO);
	tp->workers = tp_wc_init(num_workers);

	return tp;
}

void tp_destroy(thread_pool* tp)
{
	tp_job_queue_destroy(tp->jobs);
	tp_wc_destroy(tp->workers);
	
	free(tp->workers);
	free(tp->jobs);
	free(tp);
}

void tp_enqueue_request(thread_pool *tp, int fd)
{
	sem_wait(tp->jobs->empty);

	pthread_mutex_lock(tp->jobs->request_queue_mutex);
	
	tp->jobs->buffer[tp->jobs->insert_pointer] = fd;
	tp->jobs->insert_pointer++;
	tp->jobs->insert_pointer = tp->jobs->insert_pointer % tp->jobs->buffer_size;

	pthread_mutex_unlock(tp->jobs->request_queue_mutex);
	sem_post(tp->jobs->full);
		
}

int tp_dequeue_request(thread_pool *tp)
{
	int fd;

	sem_wait(tp->jobs->full);

	pthread_mutex_lock(tp->jobs->request_queue_mutex);
	
	fd = tp->jobs->buffer[tp->jobs->insert_pointer];
	tp->jobs->remove_pointer++;
	tp->jobs->remove_pointer = tp->jobs->remove_pointer % tp->jobs->buffer_size;

	pthread_mutex_unlock(tp->jobs->request_queue_mutex);
	sem_post(tp->jobs->empty);

	return fd;
		
}

pthread_t tp_add_worker(thread_pool *tp, void* (*worker_function)(void*))
{

	pthread_t tid;

	if (tp->workers->num_workers < tp->workers->max_workers) {

		if (pthread_create(&tp->workers->workers[tp->workers->num_workers], 
					NULL, worker_function, (void*) tp)) {

			/* Creating thread failed */
			tid = -1;

		}

	} else {
		/* Thread pool is at max size */
		tid = -1;
	}

	return tid;	
}
