#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "thread_pool.h"

#define TRUE 1
#define BUFFER_SIZE 8096

void parse_request(char *buffer, char **method, char **uri);
char* parse_mime(char *uri);

void* worker_function( void *tp )
{

	tp_job *job;
	thread_pool *t_pool;
	
	char *method;
	char *uri;

	int fd;
	ssize_t	bytes_read;

	char BUFFER[BUFFER_SIZE+1];

	/* Cast parameter to thread pool */
	t_pool = (thread_pool*) tp;

	/* Continously iterate over available requests */
	while (TRUE) {

		/* Block on available request */
		job = tp_dequeue_request( t_pool );

		read(job->sockfd, BUFFER, BUFFER_SIZE);

		parse_request(BUFFER, &method, &uri);
		fprintf(stderr, "Recieved %s request for %s\n", method, uri);

		if ( !strcmp(method, "GET") ) {
			
			fd = open(uri+1, O_RDONLY);

			if ( fd > 0 ) {

				bytes_read = sprintf(BUFFER, "HTTP/1.0 200 OK\r\nContent-Type: %s\r\n\r\n", parse_mime(uri));
				write(job->sockfd, BUFFER, bytes_read);

				while ((bytes_read = read(fd, BUFFER, BUFFER_SIZE)) > 0 ) {
					write(job->sockfd, BUFFER, bytes_read);
				}

				close(fd);

			} else { 
				write(job->sockfd, "HTTP/1.0 404 NOT FOUND\r\n", 24);
			}
		} else {
			write(job->sockfd, "HTTP/1.0 405 METHOD NOT SUPPORTED\r\n", 36);
		}

		/* Cleanup */
		close(job->sockfd);
		tp_job_destroy(job);
	}

}

char* parse_mime(char *uri)
{
	char* ext;

	ext = strrchr(uri, '.') + 1;

	if ( strcmp(ext, "html") || strcmp(ext, "htm") ) 
		return "text/html";

	if (strcmp(ext, "gif"))
		return "image/gif";

	if (strcmp(ext, "jpg"))
		return "image/jpg";

	if (strcmp(ext, "png"))
		return "image/png";

	return "application/octet-stream";
}

void parse_request(char *buffer, char **method, char **uri)
{

	char *copy_buffer;
	char *token, *strtok_saveptr;

	copy_buffer = malloc(strlen(buffer) * sizeof(char) + 1);
	strcpy(copy_buffer, buffer);

	token = strtok_r(copy_buffer, " ", &strtok_saveptr);
	*method = malloc(strlen(token) * sizeof(char) + 1);
	strcpy(*method, token);

	token = strtok_r(NULL, " ", &strtok_saveptr);
	*uri = malloc(strlen(token) * sizeof(char) + 1);
	strcpy(*uri, token);

	free(copy_buffer);

}
