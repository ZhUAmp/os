CC = cc
CFLAGS = -Wall -pthread

all: client

client: client.o threadpool.o
	$(CC) $(CFLAGS) -o client client.o threadpool.o

client.o: client.c threadpool.h
	$(CC) $(CFLAGS) -c client.c

threadpool.o: threadpool.c threadpool.h
	$(CC) $(CFLAGS) -c threadpool.c

clean:
	rm -f *.o client
