CFLAGS=-g -Wall -ansi -pedantic
CC=gcc

.PHONY: clean

webserver: webserver.c
	$(CC) $(CFLAGS) -o webserver webserver.c
clean:
	/bin/rm -f webserver *.o
