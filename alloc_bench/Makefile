# Makefile for GNU make

LDLIBS = -pthread
CC = g++

CFLAGS = -g -std=c++11 -O2 -Wall -pthread

OBJECTS = alloc_bench.o


main: $(OBJECTS) 
	$(CC) $(CFLAGS) -o $@ $(OBJECTS) $(LDLIBS)

%.o : %.cpp
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o main *~
