# Makefile for GNU make

LDLIBS = -lm
CC = g++
CFLAGS = -g -O2 -Wall

OBJECTS = main.o PPQ.o SkipList.o

main: $(OBJECTS) 
	$(CC) $(CFLAGS) -o $@ $(OBJECTS) $(LDLIBS)

%.o : %.cpp %.hpp
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o main *~
