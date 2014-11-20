# Makefile for GNU make

LDLIBS = -pthread -lboost_program_options
CC = g++

# For compiling in OSX
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
  CC = clang++
endif

CFLAGS = -g -std=c++11 -O2 -Wall -pthread -stdlib=libc++

OBJECTS = main.o #PPQ.o SkipList.o

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
