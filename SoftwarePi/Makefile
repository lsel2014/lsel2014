CC = gcc
CFLAGS = -g -Wall $(shell xeno-config --skin native --cflags)
LDFLAGS = $(shell xeno-config --skin native --ldflags)
LDLIBS = -lwiringPi -lpthread -lncurses -lm

daemon: Tasks/trainCtrl.o Tasks/poll.o Tasks/sun.o daemon.o

clean:
	$(RM) *.o *~ daemon Tasks/*.o Tasks/*~
