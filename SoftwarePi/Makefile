ARCH = $(shell uname -m)

ifeq ($ARCH,x86_64)
CC = gcc
CFLAGS = -g -Wall $(shell xeno-config --skin native --cflags)
LDFLAGS = $(shell xeno-config --skin native --ldflags)
LDLIBS = -lwiringPi -lreadline -lm
ARCH_OBJS = screen.o
else
CC = gcc
CFLAGS = -g -Wall -I.
LDFLAGS = 
LDLIBS = -lreadline -lm
ARCH_OBJS = wiringPi.o
endif

objs = \
	daemon.o \
	task.o \
	interp.o \
	model.o observer.o \
	plugin.o \
	utils.o \
	$(ARCH_OBJS)

$(objs): CFLAGS+=-fPIC
$(objs): LDFLAGS+=-rdynamic

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

all: daemon

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

daemon: $(objs)

clean:
	$(RM) *.o *~ daemon test/*.out

.PHONY: test
