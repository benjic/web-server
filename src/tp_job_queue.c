#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>

#include "thread_pool.h"

tp_job_queue* tp_job_queue_init(int buffer_size)
{
	int err;

	tp_job_queue* tp;

	tp = malloc(sizeof(tp_job_queue));

	tp->request_queue_mutex = malloc(sizeof(pthread_mutex_t));
	tp->empty  = malloc(sizeof(sem_t));
	tp->full   = malloc(sizeof(sem_t));
	tp->buffer = malloc(sizeof(int) * buffer_size);

	tp->buffer_size = buffer_size;
	tp->insert_pointer = 0;
	tp->remove_pointer = 0;

	err = pthread_mutex_init(tp->request_queue_mutex, NULL);
	
	if ( err )
		perror("mutex");

	err = sem_init(tp->empty, 0, buffer_size);

	if ( err ) 
		perror("empty sem:");

	err = sem_init(tp->full, 0, 0);

	if ( err ) 
		perror("empty sem:");

	return tp;
}

void tp_job_queue_destroy(tp_job_queue *tp)
{
	int err;

	err = pthread_mutex_destroy(tp->request_queue_mutex);
	
	if ( err )
		perror("mutex");

	err = sem_destroy(tp->empty);

	if ( err ) 
		perror("empty sem:");

	err = sem_destroy(tp->full);

	if ( err ) 
		perror("full sem:");

	free(tp->request_queue_mutex);
	free(tp->full);
	free(tp->empty);

	free(tp->buffer);

	free(tp);
}
