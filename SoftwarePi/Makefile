CC = gcc
CFLAGS = -g -Wall $(shell xeno-config --skin native --cflags)
LDFLAGS = $(shell xeno-config --skin native --ldflags)
LDLIBS = -lwiringPi -lpthread -lncurses -lm

main_VelBar: main_VelBar.o controlvelocidad.o

clean:
	$(RM) *.o *~ main_VelBar
