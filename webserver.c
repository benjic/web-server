#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>

int main(int argc, char** argv)
{

	int listen_fd;

	struct sockaddr_in serv_addr;

	listen_fd = socket(AF_INET, SOCK_STREAM, 0);

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(8080);

	bind(listen_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
	
	if (listen(listen_fd, 64)) {
		perror("listen");
	} else {
		
	}

	return 0;
}
