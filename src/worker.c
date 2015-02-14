#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "thread_pool.h"

/* Magic Numbers */
#define TRUE 1
#define BUFFER_SIZE 8096

/* Helper prototypes */
void parse_request(char *buffer, char **method, char **uri);
char* parse_mime(char *uri);

/*
 * Default worker function that consumes jobs in the job queue. As defined in
 * the worker collection, it is passed a pointer to the thread pool to allow the
 * worker to access data enqueued.
 */
void* worker_function( void *tp )
{

	/* Important structs */
	tp_job *job;
	thread_pool *t_pool;
	
	/* Parsed data */
	char *method;
	char *uri;

	/* Variables used to read files */
	int fd;
	ssize_t	bytes_read;

	/* Define buffer */
	char BUFFER[BUFFER_SIZE+1];

	/* Cast parameter to thread pool */
	t_pool = (thread_pool*) tp;

	/* Continously iterate over available requests */
	while (TRUE) {

		/* Block on available request */
		job = tp_dequeue_request( t_pool );

		/* We read the first BUFFER_SIZE bytes which is not typical, there may be
		 * more data included we are ignoring. 
		 * */
		read(job->sockfd, BUFFER, BUFFER_SIZE);

		/* Look at buffer to grab the relative important variables to return a
		 * result
		 */
		parse_request(BUFFER, &method, &uri);

		/* Test to make sure parse request did not not return null pointers
		 * resulting in malformed requests. If we get any type of request that
		 * can't be parsed then the request is bad.
		 */
		if (!method || !uri) {

			/* Return status code for a bad request from client as it is malformed */
			write(job->sockfd, "HTTP/1.0 400 Bad Request\r\n", 36);

			/* Cleanup */
			close(fd);
			close(job->sockfd);
			tp_job_destroy(job);

			/* Start a new iteration */
			continue;
		}

		/* We only serve GET requests */
		if (!strcmp(method, "GET")) {

			/* Alert daemon to incoming request */
			fprintf(stderr, "Recieved %s request for %s\n", method, uri);
			
			/* Try to open the given uri. It is worth noting that I removed the first
			 * character from the uri becuase it is given /dfasdf/asdfsd.htm and the
			 * file path would relative to process not root of the filesystem.
			 */
			fd = open(uri+1, O_RDONLY);

			if ( fd > 0 ) {

				/* Write the first part of the response to the socket including the
				 * content type.
				 */
				bytes_read = sprintf(BUFFER, "HTTP/1.0 200 OK\r\nContent-Type: %s\r\n\r\n", parse_mime(uri));
				write(job->sockfd, BUFFER, bytes_read);

				/* Begin reading file at BUFFER_SIZE chunks at a time */
				while ((bytes_read = read(fd, BUFFER, BUFFER_SIZE)) > 0 ) {
					/* The following is a redundant comment :)*/
					/* Write to the socket the bytes read */
					write(job->sockfd, BUFFER, bytes_read);
				}

			} else { 
				/* Respond with error when uri is a valid file */
				write(job->sockfd, "HTTP/1.0 404 NOT FOUND\r\n", 24);
			}
		} else {
			/* Respond with error when using a method not supported */
			write(job->sockfd, "HTTP/1.0 405 METHOD NOT SUPPORTED\r\n", 36);
		}

		/* Cleanup */
		close(fd);
		close(job->sockfd);
		tp_job_destroy(job);
	}

}

/*
 * Helper functiotion to get a mime time for a given uri.
 */
char* parse_mime(char *uri)
{
	char* ext;

	/* strrchr finds the right most index that matches given pattern which
	 * effectively returns the file extension substring 
	 */
	ext = strrchr(uri, '.') + 1;

	/* Check to see if it is html */
	if ( strcmp(ext, "html") || strcmp(ext, "htm") ) 
		return "text/html";

	/* Check to see if it is gif*/
	if (strcmp(ext, "gif"))
		return "image/gif";

	/* Check to see if it is jpg */
	if (strcmp(ext, "jpg"))
		return "image/jpg";

	/* Check to see if it is png*/
	if (strcmp(ext, "png"))
		return "image/png";

	/* Anything else is just bytes to be figured out by the client */
	return "application/octet-stream";
}

/*
 * Helper function to take the buffer and extractt he variables we are
 * interested in. This could be extended to produce a new struct of type request
 * that has *all* the informaiton parsed in an accesible data structure but is
 * extra work.
 *
 * This function takes a given buffer and addresses to variables that store the
 * method and uri parameters in the caller.
 */
void parse_request(char *buffer, char **method, char **uri)
{

	/* we don't want to mutate the caller buffer so a copy buffer is needed */
	char *copy_buffer;
	
	/* token represents a pointer to strtok_r and the saveptr is nessecary for
	 * strtok_r which is reentrant.
	 */
	char *token, *strtok_saveptr;

	/* Copy the given buffer to the copy buffer */
	copy_buffer = malloc(strlen(buffer) * sizeof(char) + 1);
	strcpy(copy_buffer, buffer);

	/* Begin tokenizing the copy buffer with spaces */
	token = strtok_r(copy_buffer, " ", &strtok_saveptr);

	if (!token) {
		*method = 0;
		*uri = 0;
		return;
	}

	/* The first char sequence in HTTP will be the method of the request. A new
	 * bit of heap is made available for the method variable of size of the token
	 * lenght and is then copied byte for byte with token.
	 */
	*method = malloc(strlen(token) * sizeof(char) + 1);
	strcpy(*method, token);

	/* Again token is updated */
	token = strtok_r(NULL, " ", &strtok_saveptr);

	if (!token) {
		*method = 0;
		*uri = 0;
		return;
	}

	/* 
	 * Memory is allocated for the uri and the value is coppied to this new space
	 */
	*uri = malloc(strlen(token) * sizeof(char) + 1);
	strcpy(*uri, token);

	/* The copy buffer is no longer needed and is free'd */
	free(copy_buffer);

}
