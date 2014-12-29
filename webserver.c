#include <stdlib.h>
#include <sys/socket.h>

int main(int argc, char** argv)
{

	int listen_fd;

	listen_fd = socket(AF_INET, SOCK_STREAM, 0);

	return 0;
}
