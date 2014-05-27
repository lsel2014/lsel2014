CC = gcc
CFLAGS = -g -Wall $(shell xeno-config --skin native --cflags)
LDFLAGS = $(shell xeno-config --skin native --ldflags)
LDLIBS = -lwiringPi -lreadline -lm

objs = \
	daemon.o \
	tasks.o \
	interp.o \
	train.o \
	observer.o \
	sensorIR.o \
	trafficLight.o \
	crossingGate.o \
	railChange.o \
	semaphore.o \
	dcc.o \
	sun.o sunparse.o \
	model.o \
	tracker.o \
	railway.o \
	anticollision.o \
	screen.o \
	utils.o\
        ctrlIlumination.o\
        lsquaredc.o
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

#test_trafficLight: Model/observer.o Model/Actuators/actuator.o Model/Actuators/trafficLight.o Model/Actuators/test_trafficLight.o

clean:
	$(RM) *.o *~ daemon Tasks/*.o Tasks/*~ Interpreter/*.o Interpreter/*~ Model/*.o Model/*~ Model/Actuators/*.o Model/Actuators/*~ Model/Sensors/*.o Model/Sensors/*~ test/*.out

.PHONY: test
