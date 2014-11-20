# Makefile for GNU make

CC = g++
CFLAGS = -g -O2 -std=gnu++0x -Wall -lpthread

OBJECTS = main.o PPQ.o SkipList.o


main: $(OBJECTS) 
	$(CC) $(CFLAGS) -o $@ $(OBJECTS)

%.o : %.cpp
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o main *~
