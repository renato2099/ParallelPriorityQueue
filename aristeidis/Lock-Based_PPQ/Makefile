# Makefile for GNU make

LDLIBS = -lm -lpthread
CC = g++
CFLAGS = -g -O2 -std=gnu++0x -Wall

OBJECTS = main.o PPQ.o SkipList.o


main: $(OBJECTS) 
	$(CC) $(CFLAGS) -o $@ $(OBJECTS) $(LDLIBS)

%.o : %.cpp
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o main *~
