#ifndef THREAD_POOL_H
#define THREAD_POOL_H
#include <pthread.h>
#include <semaphore.h>
#include <sys/types.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

/* A simple struct to represent an incoming request */
typedef struct tp_job_s {
	struct sockaddr *client_addr;
	socklen_t *length;
	int sockfd;
} tp_job;

/* Create and destroy jobs */
tp_job* tp_job_init();
void tp_job_destroy(tp_job*);

/* A thread safe conainer for jobs */
typedef struct tp_job_queue_s { 
	int buffer_size;
	int insert_pointer;
	int remove_pointer;
	pthread_mutex_t *request_queue_mutex;
	sem_t *empty;
	sem_t *full;
	tp_job **buffer;
} tp_job_queue;

tp_job_queue *tp_job_queue_init(int buffer_size);
void tp_job_queue_destroy(tp_job_queue *tp_j);

/* A simple container for threads */
typedef struct tp_worker_collection_s {
	int num_workers;
	int max_workers;
	pthread_t *workers;
} tp_worker_collection;

/* General structure of thread collections and job queue */
typedef struct thread_pool_s {
	tp_job_queue *jobs;
	tp_worker_collection *workers;
} thread_pool;

tp_worker_collection *tp_wc_init(int num_workers);
void tp_wc_destroy(tp_worker_collection *tp_w);


/* Functions related to manipulating the thread pool and job queue within it */
void tp_destroy(thread_pool* tp);
thread_pool* tp_init(int buffer_size);

void tp_enqueue_request(thread_pool *tp, tp_job *job);
tp_job* tp_dequeue_request(thread_pool *tp);

/* Function for adding a worker using the given function */
pthread_t tp_add_worker(thread_pool *tp, void* (*)(void*));

#endif
