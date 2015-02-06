CFLAGS=-g -Wall -ansi -pedantic -lrt -lpthread
CC=gcc

OBJECTS=thread_pool.o tp_job_queue.o webserver.o

.PHONY: clean
%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<
webserver: $(OBJECTS)
	$(CC) $(CFLAGS) -o webserver $(OBJECTS)
clean:
	/bin/rm -f webserver *.o
