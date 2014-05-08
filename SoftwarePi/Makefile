ARCH = $(shell uname -m)

ifeq ($ARCH,x86_64)
CC = gcc
CFLAGS = -g -Wall -fPIC $(shell xeno-config --skin native --cflags)
LDFLAGS = -rdynamic $(shell xeno-config --skin native --ldflags)
LDLIBS = -lwiringPi -lreadline -lm -ldl
ARCH_OBJS = screen.o
else
CC = gcc
CFLAGS = -g -Wall -I. -fPIC
LDFLAGS = -fPIC -rdynamic
endif

OBJS = daemon.o

libsel_OBJS = \
	task.o \
	interp.o \
	plugin.o \
	model.o observer.o \
	utils.o

other = \
	train.o railway.o \
	sensor.o \
	sensorIR.o \
	actuator.o \
	trafficLight.o \
	crossingGate.o \
	railChange.o \
	dcc.o \
	sun.o sunparse.o \
	tracker.o anticollision.o \
	time_operations.o

all: libwiringPi.so libsel.so daemon train.so

daemon: LDLIBS = -L. -lsel -lwiringPi

libsel.so: LDLIBS = -lreadline -lm -L. -lwiringPi
libsel.so: $(libsel_OBJS)

libwiringPi.so: wiringPi.o

train.so: LDLIBS = -L. -lsel -lwiringPi
train.so: train.o dcc.o

test: daemon
	@for i in test/*.test ; do \
	  printf "%-30s" $$i ; \
	  LINES=24 COLUMNS=80 ./daemon < $$i 2>&1 > $$i.out ; \
	  if cmp $$i.out $$i.exp 2>&1 >/dev/null; then \
	    echo OK ; \
	  else \
	    echo FAIL ; \
	  fi ; \
	done

daemon: $(OBJS)

%.so:
	$(CC) -shared $(LDFLAGS) -o $@ $^ $(LDLIBS)

clean:
	$(RM) *.o *.so *~ daemon test/*.out

.PHONY: test
