CC = gcc
CFLAGS = -g -Wall $(shell xeno-config --skin native --cflags)
LDFLAGS = $(shell xeno-config --skin native --ldflags)
LDLIBS = -lwiringPi -lpthread -lreadline -lncurses -lm

daemon: dcc.o train.o Tasks/sunTasks.o Interpreter/interp.o daemon.o

clean:
	$(RM) *.o *~ daemon Tasks/*.o Tasks/*~ Interpreter/*.o Interpreter/*~
