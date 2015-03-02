#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include "thread_pool.h"
#define BUFFER_RATIO 5

/*
 * Initilize a new thread pool with a given max number of worker threads given
 * as num_workers.
 */
thread_pool *tp_init(int num_workers)
{
	thread_pool *tp;

	tp = malloc(sizeof(thread_pool));

	/* Ask for memory allocation for queue and thread pool */
	tp->jobs = tp_job_queue_init(num_workers * BUFFER_RATIO);
	tp->workers = tp_wc_init(num_workers);

	return tp;
}

/*
 * Take care of releasing memory when done with thread pool passed as
 * *thread_pool tp.
 */
void tp_destroy(thread_pool* tp)
{
	/* Helper functions to release memory for the job queue and thread pool */
	tp_job_queue_destroy(tp->jobs);
	tp_wc_destroy(tp->workers);
	
	/* Free field members */
	free(tp->workers);
	free(tp->jobs);

	/* Free given pointer */
	free(tp);
}

/* 
 * Pass a new job to queue to be enqueued in thread safe manner. Takes pointer
 * to a given thread pool as well as the new job to enqueu.
 */
void tp_enqueue_request(thread_pool *tp, tp_job *job)
{

	/* Block and sleep when buffer is full */
	sem_wait(tp->jobs->empty);

	/* Gain exclusive access to buffer */
	pthread_mutex_lock(tp->jobs->request_queue_mutex);
	
	/* Insert job in next available spot */
	tp->jobs->buffer[tp->jobs->insert_pointer] = job;

	/* Update pointer to next spot */
	tp->jobs->insert_pointer++;
	tp->jobs->insert_pointer = tp->jobs->insert_pointer % tp->jobs->buffer_size;

	/* Release lock */
	pthread_mutex_unlock(tp->jobs->request_queue_mutex);

	/* Increment full semaphore */
	sem_post(tp->jobs->full);
		
}

tp_job* tp_dequeue_request(thread_pool *tp)
{
	tp_job *job;

	/* Block until job is available */
	sem_wait(tp->jobs->full);

	/* Gain exlcusive access to buffer */
	pthread_mutex_lock(tp->jobs->request_queue_mutex);
	
	/* Copy pointer in buffer */
	job = tp->jobs->buffer[tp->jobs->remove_pointer];

	/* Increment dequeue pointer and wrap buffer size*/
	tp->jobs->remove_pointer++;
	tp->jobs->remove_pointer = tp->jobs->remove_pointer % tp->jobs->buffer_size;

	/* Give back exslusive access */
	pthread_mutex_unlock(tp->jobs->request_queue_mutex);
	
	/* Increase semaphore for empty */
	sem_post(tp->jobs->empty);

	return job;
		
}

/*
 * Here is a doozy.
 *
 * This helper function handles spawning a new thread with the given function
 * pointer. The given thread_pool tp gains a pthread_t in the worker collection
 * in the event you would want to mess with the threads later on.
 */
pthread_t tp_add_worker(thread_pool *tp, void* (*worker_function)(void*))
{

	pthread_t tid;

	/* Make sure we have room for more workers */
	if (tp->workers->num_workers < tp->workers->max_workers) {

		/* Try to make a thread and store tid in worker collection */
		if (pthread_create(
					&tp->workers->workers[tp->workers->num_workers++], /* Address of pthread_t location */
					NULL,            /*  Hmm? See man pthread_create... */
					worker_function, /*  Pointer to function to run */
					(void*) tp))     /*  Pass the pointer to the thread_pool to the worker function */
		{

			/* Creating thread failed */
			tid = -1;

		}

	} else {
		/* Thread pool is at max size */
		tid = -1;
	}

	return tid;	
}

/* 
 * Memory helper for creating job structs
 */
tp_job* tp_job_init()
{
	tp_job *job;

	/* Create memory position for job */
	job = malloc(sizeof(tp_job));

	/* Field allocations */
	job->client_addr = malloc(sizeof(struct sockaddr_in));
	job->length	= malloc(sizeof(socklen_t));

	/* Default value */
	job->sockfd = 0;

	return job;
}

/*
 * Memory de-allocation helper for jobs
 */
void tp_job_destroy(tp_job *job)
{
	/* Release field members */
	free(job->client_addr);
	free(job->length);

	/* Release pointer */
	free(job);
}
