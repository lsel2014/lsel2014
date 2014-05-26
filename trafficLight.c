/*
 LSEL 2014

 @authors _____, J. Martin
 @date April 2014

 Represents a traffic semaphore that activates when some trains are in certain sectors with a certain direction.

 */

#include <stdbool.h>
#include <wiringPi.h>

#include "trafficLight.h"
#include "train.h"
//static trafficLight_t* light;
trafficLight_t* trafficLights[MAXTRAFFICLIGHTS];
int n_trafficLights;

void trafficLight_setup(void) {

	 //trafficLight_t* dummy = trafficLight_new(0, 12);
	 trafficLight_new(0, 13);
	 interp_addcmd("semaphore", trafficLight_cmd, "Set semaphore state\n");
	 
	 
}

int trafficLight_cmd(char* arg) {
    int i;
	if (0 == strcmp(arg, "list")) {
        for( i= 0; i<n_trafficLights;i++)
		printf("Semaphore\t%s\r\n", (trafficLights[i]->state) == ON ? "ON" : "OFF");

		return 0;
	}
	if (0 == strncmp(arg, "set ", strlen("set "))) {
		int state;
		state = atoi(arg + strlen("set "));
		for( i= 0; i<n_trafficLights;i++)
		trafficLight_set_state(trafficLights[i], state == 1 ? ON : OFF);
		return 1;
	}
	return 1;
}

void trafficLight_init(trafficLight_t* this, int id, int GPIOline,
		state_t state) {
        observable_init(&this->observable);        
	this->state = state;
	this->GPIOline = GPIOline;
	rt_mutex_create(&this->mutex, NULL);

	// Set GPIO line as output
	pinMode(this->GPIOline, OUTPUT);
	trafficLight_set_state(this, state);

}

trafficLight_t*
trafficLight_new(int id, int GPIOline) {
	trafficLight_t* this = (trafficLight_t*) malloc(sizeof(trafficLight_t));
	trafficLight_init(this, id, GPIOline, OFF);
    if (n_trafficLights < MAXTRAFFICLIGHTS) {
		trafficLights[n_trafficLights++] = this;
	}
	return this;

}

state_t trafficLight_get_state(trafficLight_t* this) {

	return this->state;

}

void trafficLight_set_state(trafficLight_t* this, state_t state) {
	rt_mutex_acquire(&(this->mutex), TM_INFINITE);
	this->state = state;
	if (state == ON)
		digitalWrite(this->GPIOline, 1);
	else
		digitalWrite(this->GPIOline, 0);
		
	rt_mutex_release(&(this->mutex));

}
