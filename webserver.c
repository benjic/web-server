#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#include "thread_pool.h"

#define TRUE 1
#define NUM_WORKERS 3

int main(int argc, char** argv)
{

	int listen_fd;

	socklen_t length;

	int err;
	unsigned int port;

	struct sockaddr_in serv_addr;
	struct sockaddr_in client_addr;

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

	 	tp_enqueue_request(t_pool, accept(listen_fd, (struct sockaddr*)&client_addr, &length));
		fprintf(stderr, "DEBUG: Incoming request was accepted\n");
	 	
	 }

	return 0;
}
