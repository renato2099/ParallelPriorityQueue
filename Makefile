# Makefile for GNU make

LDLIBS = -pthread -lboost_program_options -ltbb -ltbbmalloc
CC = g++

# For compiling in OSX
#UNAME_S := $(shell uname -s)
#ifeq ($(UNAME_S),Darwin)
#  CC = clang++
#endif

CFLAGS = -g -std=c++11 -O2 -Wall -pthread

OBJECTS = main.o #PPQ.o SkipList.o

main: $(OBJECTS) 
	$(CC) $(CFLAGS) -o $@ $(OBJECTS) $(LDLIBS)

%.o : %.cpp
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o main *~
