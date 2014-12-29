#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

#define TRUE 1
#define BUFFER_SIZE 8096

int main(int argc, char** argv)
{

	int listen_fd;
	int connection_fd;

	socklen_t length;

	int err;
	int read_len;

	char buffer[BUFFER_SIZE+1];

	struct sockaddr_in serv_addr;
	struct sockaddr_in client_addr;

	listen_fd = socket(AF_INET, SOCK_STREAM, 0);

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(8080);

	err = bind(listen_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

	if (err == -1) {
		perror("bind");
	}

	err = listen(listen_fd, 64);

	if (err == -1) {
		perror("listen");
	}

	while (TRUE) {

		connection_fd = accept(listen_fd, (struct sockaddr*)&client_addr, &length);

		read_len = read(connection_fd, buffer, BUFFER_SIZE);
		printf("Read %d bytes: %s\n", read_len,  buffer);
		
		close(connection_fd);
	}

	return 0;
}
