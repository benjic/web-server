#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#include "thread_pool.h"

#define TRUE 1
#define NUM_WORKERS 3

/* Prototypes */
void* worker_function( void *tp );

int main(int argc, char** argv)
{
	/* Local varialbes */
	int i;

	int err;
	unsigned int port;

	/* Structs for network info */
	int listen_fd;
	struct sockaddr_in serv_addr;

	/* Define thread pool */
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

	/* Intialize thread pool structure */
	t_pool = tp_init(NUM_WORKERS);

	/* Spawn NUM_WORKER threads to handle requests */
	for ( i = 0; i < NUM_WORKERS; i++ ) {
		if (!(tp_add_worker(t_pool, worker_function))) {

			/* tp_add_worker returns 0 on error */
			perror("Cannot create worker function");

		}
	}

	/* Create listening socket for incoming connecions */
	listen_fd = socket(AF_INET, SOCK_STREAM, 0);

	/* Define IPv4 info for incoming socket */
	serv_addr.sin_family = AF_INET;

	/* Bind to any address on machine */
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	/* Use the configured port for listening */
	serv_addr.sin_port = htons(port);

	/* Attempt to bind the socket with specificed informaiton */
	err = bind(listen_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

	/* Bind failed ? */
	if (err == -1) {
		perror("bind");
	}

	/* Confirm read to take packets on given socket */
	err = listen(listen_fd, 32);

	/* Did listen fail? */
	if (err == -1) {
		perror("listen");
	}

	/* Alert user that webserver is now working */
	printf("Webserver on port %d is listening for connections\n", port);

	/* Try infinitely */
	while (TRUE) {

		/* Create new job */
		tp_job *job;
		job = tp_job_init();
		
		/* Wait for incoming connection */
		job->sockfd = accept(listen_fd, 
				job->client_addr,
				job->length);

		/* Add request to job queue */
		tp_enqueue_request(t_pool, job);
	 	
	 }

	return 0;
}
