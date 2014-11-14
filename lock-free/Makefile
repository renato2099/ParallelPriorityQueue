# Makefile for GNU make

LDLIBS = -pthread -lboost_program_options
CC = g++
CFLAGS = -g -std=c++11 -O2 -Wall -pthread

OBJECTS = main.o LockFreeSkipList.o

main: $(OBJECTS) 
	$(CC) $(CFLAGS) -o $@ $(OBJECTS) $(LDLIBS)

%.o : %.cpp
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o main *~
