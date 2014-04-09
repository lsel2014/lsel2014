CC = gcc
CFLAGS = -g -Wall $(shell xeno-config --skin native --cflags)
LDFLAGS = $(shell xeno-config --skin native --ldflags)
LDLIBS = -lwiringPi -lpthread -lreadline -lm

daemon: Model/observer.o Model/Sensors/sensorIR.o Model/Sensors/sensor.o Model/Actuators/actuator.o Model/Actuators/trafficLight.o Model/Actuators/crossingGate.o Model/Actuators/railChange.o dcc.o sun.o sunparse.o train.o tasks.o Tasks/sunTasks.o Interpreter/interp.o daemon.o

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

#test_trafficLight: Model/observer.o Model/Actuators/actuator.o Model/Actuators/trafficLight.o Model/Actuators/test_trafficLight.o

clean:
	$(RM) *.o *~ daemon Tasks/*.o Tasks/*~ Interpreter/*.o Interpreter/*~ Model/*.o Model/*~ Model/Actuators/*.o Model/Actuators/*~ Model/Sensors/*.o Model/Sensors/*~
