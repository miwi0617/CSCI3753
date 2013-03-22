#Heather Dykstra
#Due: 2/27/2013
#The following is an edit of the given Makefile
#I just added my multi-look compile lines
CC = gcc
CFLAGS = -c -g -Wall -Wextra -O3  
LFLAGS = -Wall -Wextra -pthread  

.PHONY: all clean

all: lookup queueTest pthread-hello multi-lookup

lookup: lookup.o queue.o util.o
	$(CC) $(LFLAGS) $^ -o $@

queueTest: queueTest.o queue.o
	$(CC) $(LFLAGS) $^ -o $@

pthread-hello: pthread-hello.o
	$(CC) $(LFLAGS) $^ -o $@

multi-lookup: multi-lookup.o queue.o util.o 
	$(CC) $(LFLAGS) $^ -o $@

lookup.o: lookup.c
	$(CC) $(CFLAGS) $<

queueTest.o: queueTest.c
	$(CC) $(CFLAGS) $<

queue.o: queue.c queue.h
	$(CC) $(CFLAGS) $<

util.o: util.c util.h
	$(CC) $(CFLAGS) $<

pthread-hello.o: pthread-hello.c
	$(CC) $(CFLAGS) $<

multi-lookup.o: multi-lookup.c multi-lookup.h
	$(CC) $(CFLAGS) $<



clean:
	rm -f lookup queueTest pthread-hello multi-lookup
	rm -f *.o
	rm -f *~
	rm -f results.txt
