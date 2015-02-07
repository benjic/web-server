CFLAGS=-g -Wall -ansi -pedantic -lrt -lpthread
CC=gcc

THRD_POOL_OBJS=thread_pool.o tp_job_queue.o tp_worker_collection.o
WEBSRV_OBJS=webserver.o worker.o
OBJECTS=$(THRD_POOL_OBJS) $(WEBSRV_OBJS)

.PHONY: clean
%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<
webserver: $(OBJECTS)
	$(CC) $(CFLAGS) -o webserver $(OBJECTS)
clean:
	/bin/rm -f webserver *.o
