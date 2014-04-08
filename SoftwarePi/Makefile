CC = gcc
CFLAGS = -g -Wall $(shell xeno-config --skin native --cflags)
LDFLAGS = $(shell xeno-config --skin native --ldflags)
LDLIBS = -lwiringPi -lpthread -lreadline -lm

daemon: Model/observer.o Model/Sensors/sensorIR.o Model/Sensors/sensor.o Model/Actuators/actuator.o Model/Actuators/trafficLight.o Model/Actuators/crossingGate.o dcc.o sun.o sunparse.o train.o tasks.o Tasks/sunTasks.o Interpreter/interp.o daemon.o

clean:
	$(RM) *.o *~ daemon Tasks/*.o Tasks/*~ Interpreter/*.o Interpreter/*~ Model/*.o Model/*~ Model/Actuators/*.o Model/Actuators/*~ Model/Sensors/*.o Model/Sensors/*~
