ARCH = $(shell uname -m)

ifneq ($(ARCH),x86_64)
CC = gcc
CFLAGS = -g -Wall -fPIC $(shell xeno-config --skin native --cflags)
LDFLAGS = -rdynamic $(shell xeno-config --skin native --ldflags)
ARCH_OBJS = screen.o
else
CC = gcc
CFLAGS = -g -Wall -I. -fPIC
LDFLAGS = -fPIC -rdynamic
ARCH_TARGETS = libwiringPi.so
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
	model.o \
	tracker.o anticollision.o \
	railway.o \
	screen.o \
	time_operations.o \
	logger.o

all: $(ARCH_TARGETS) libsel.so daemon train.so

daemon: LDLIBS = -L. -lsel -lwiringPi

libwiringPi.so: wiringPi.o

libsel.so: LDLIBS = -lreadline -lm -L. -lwiringPi -ldl
libsel.so: $(libsel_OBJS)

train.so: LDLIBS = -L. -lsel -lwiringPi
train.so: train.o dcc.o

test: daemon
	@cat test/init.txt ; read foo
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
