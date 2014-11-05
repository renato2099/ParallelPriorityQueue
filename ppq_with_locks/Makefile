# Makefile for GNU make

LDLIBS = -lm -lpthread
CC = g++
CFLAGS = -g -std=gnu++0x -O2 -Wall

OBJECTS = main.o PPQ.o SkipList.o

ifdef version
  ifeq "$(version)" "par"
    CFLAGS += -DTHREAD_SAFE
  endif
endif

main: $(OBJECTS) 
	$(CC) $(CFLAGS) -o $@ $(OBJECTS) $(LDLIBS)

%.o : %.cpp
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o main *~
