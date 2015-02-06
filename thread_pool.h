#ifndef THREAD_POOL_H
#define THREAD_POOL_H
#include <pthread.h>
#include <semaphore.h>

typedef struct tp_job_queue_s { 
	int buffer_size;
	int insert_pointer;
	int remove_pointer;
	pthread_mutex_t *request_queue_mutex;
	sem_t *empty;
	sem_t *full;
	int *buffer;
} tp_job_queue;

tp_job_queue *tp_job_queue_init(int buffer_size);
void tp_job_queue_destroy(tp_job_queue *tp_j);

typedef struct thread_pool_s {
	tp_job_queue *jobs;
} thread_pool;

void tp_destroy(thread_pool* tp);
thread_pool* tp_init(int buffer_size);

void tp_enqueue_request(thread_pool *tp, int fd);
int tp_dequeue_request(thread_pool *tp);

#endif
