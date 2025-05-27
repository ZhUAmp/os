CC=gcc
CFLAGS=-Wall -g

OBJS_COMMON=driver.o cpu.o list.o

all: fcfs sjf priority rr prio_rr

fcfs: schedule_fcfs.o $(OBJS_COMMON)
	$(CC) $(CFLAGS) -o fcfs schedule_fcfs.o $(OBJS_COMMON)

sjf: schedule_sjf.o $(OBJS_COMMON)
	$(CC) $(CFLAGS) -o sjf schedule_sjf.o $(OBJS_COMMON)

priority: schedule_priority.o $(OBJS_COMMON)
	$(CC) $(CFLAGS) -o priority schedule_priority.o $(OBJS_COMMON)

rr: schedule_rr.o $(OBJS_COMMON)
	$(CC) $(CFLAGS) -o rr schedule_rr.o $(OBJS_COMMON)

prio_rr: schedule_prio_rr.o $(OBJS_COMMON)
	$(CC) $(CFLAGS) -o prio_rr schedule_prio_rr.o $(OBJS_COMMON)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o fcfs sjf priority rr prio_rr
