# Makefile for GNU make

LDLIBS = -pthread -ltbb -ltbbmalloc -ltbbmalloc_proxy
CC = icpc

# For compiling in OSX
#UNAME_S := $(shell uname -s)
#ifeq ($(UNAME_S),Darwin)
#  CC = clang++
#endif

CFLAGS = -g -std=c++11 -O2 -Wall -pthread 

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
