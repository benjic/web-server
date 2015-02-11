#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>

#include "thread_pool.h"

/*
 * Thread pool helper function to initilize a job queue structure 
 */
tp_job_queue* tp_job_queue_init(int buffer_size)
{
	/* Local variables */
	int err;
	tp_job_queue* tp;

	/* Create pointer to queue */
	tp = malloc(sizeof(tp_job_queue));

	/* Reserve allocation for field members */
	tp->request_queue_mutex = malloc(sizeof(pthread_mutex_t));
	tp->empty  = malloc(sizeof(sem_t));
	tp->full   = malloc(sizeof(sem_t));
	tp->buffer = malloc(sizeof(int) * buffer_size);

	/* Intialize default values */
	tp->buffer_size = buffer_size;
	tp->insert_pointer = 0;
	tp->remove_pointer = 0;

	/* Create POSIX mutex */
	err = pthread_mutex_init(tp->request_queue_mutex, NULL);
	
	/* Confirm mutex is valid from pthread */
	if ( err )
		perror("mutex");

	/* Create empty semaphore */
	err = sem_init(tp->empty, 0, buffer_size);

	/* Confirm empty semaphore is valid */
	if ( err ) 
		perror("empty sem:");

	/* Create full semaphore */
	err = sem_init(tp->full, 0, 0);

	/* Confirm full semaphore is valid */
	if ( err ) 
		perror("empty sem:");

	return tp;
}

/*
 * Memory helper function to deallocate memory for the job queue
 */
void tp_job_queue_destroy(tp_job_queue *tp)
{
	int err;

	/* Cleanup mutex */
	err = pthread_mutex_destroy(tp->request_queue_mutex);
	
	if ( err )
		perror("mutex");

	/* Destroy semaphore for empty*/
	err = sem_destroy(tp->empty);

	if ( err ) 
		perror("empty sem:");

	/* Destroy semaphore for full */
	err = sem_destroy(tp->full);

	if ( err ) 
		perror("full sem:");

	/* Free field members */
	free(tp->request_queue_mutex);
	free(tp->full);
	free(tp->empty);

	/* Release memory for buffer */
	free(tp->buffer);

	/* Release pointer */
	free(tp);
}
