#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#include "thread_pool.h"

#define TRUE 1
#define NUM_WORKERS 3

void* worker_function( void *tp )
{
	int fd;
	thread_pool *parent_tp;

	parent_tp = (thread_pool*) tp;

	while (TRUE) {

		fd = tp_dequeue_request(parent_tp);
		printf("Hello from worker: there are %d items in the work queue\n!", fd);

		sleep(5);
	}
}

int main(int argc, char** argv)
{
	/* Local varialbes */
	int i;

	socklen_t length;

	int err;
	unsigned int port;

	/* Structs for network info */
	int listen_fd;
	struct sockaddr_in serv_addr;

	thread_pool *t_pool;

	/* User specified port? */
	if (argc > 1) {
		
		/* Parse integer */
		port = atoi(argv[1]);

		/* Determine valid port number was found */
		if ( port < 1 ) {
			printf("%s is not a valid port\n", argv[1]);
			exit(-1);
		}

	} else {
		/* Default bind port */
		port = 8080;
	}

	t_pool = tp_init(NUM_WORKERS);

	for ( i = 0; i < NUM_WORKERS; i++ ) {
		if (tp_add_worker(t_pool, worker_function)) {
			perror("Cannot create worker function");
		}

	}

	listen_fd = socket(AF_INET, SOCK_STREAM, 0);

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(port);

	err = bind(listen_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

	if (err == -1) {
		perror("bind");
	}

	err = listen(listen_fd, 32);

	if (err == -1) {
		perror("listen");
	}

	printf("Webserver on port %d is listening for connections\n", port);

	while (TRUE) {

		/* Create new job */
		tp_job *job;
		job = tp_job_init();
		
		job->sockfd = accept(listen_fd, 
				job->client_addr,
				job->length);

		tp_enqueue_request(t_pool, job);
		fprintf(stderr, "DEBUG: Incoming request was accepted\n");
	 	
	 }

	return 0;
}
