/*
 * sensorIR.c
 *
 *  Created on: 02/04/2014
 *      Author: Javier
 */

#include "sensorIR.h"
#include <rtdk.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <native/mutex.h>
#include <wiringPi.h>

/*
 #define COLOR_RED "\x1b[31m"
 #define COLOR_GREEN "\x1b[32m"
 #define COLOR_RESET "\x1b[0m"

 static void sensorIR_isr0 (void)
 {
 printf(COLOR_GREEN"Detectado diesel\n"COLOR_RESET);
 //we will probably need to control rebounds and false calls
 //detected diesel train, do something

 //process data?
 //notify observers?
 //maybe just 1 isr?
 }

 static void sensorIR_isr1 (void)
 {
 printf(COLOR_GREEN"Detectado renfe\n"COLOR_RESET);
 //we will probably need to control rebounds and false calls
 //detected renfe train, do something
 }

 static void (*sensorIR_isr[]) (void) = {
 sensorIR_isr0,
 sensorIR_isr1,
 };
 */

sensorIR_t* sensors[MAXSENSORS];
int nsensors = 0;

void IRsensors_poll(void* arg) {
	sensorIR_t** sensors = (sensorIR_t**) arg;
	rt_task_set_periodic(NULL, TM_NOW, IR_PERIOD);
	while (1) {
		int i = 0;
		rt_task_wait_period(NULL);
		for (i = 0; i < nsensors; i++) {
			sensorIR_trainPassing(sensors[i]);
		}
	}
}

int 
sensors_cmd(char*arg){
	int i;
	for(i=0;i<nsensors;i++){
		printf("Sensor %d",sensors[i]->id );
	}
	return 0;
}

void IRsensors_setup(void) {
	int i;
	for (i = 0; i < 4; i++) {
		sensorIR_new(i);
	}
	task_add("IR polling", IR_DEADLINE, IRsensors_poll, sensors);
	interp_addcmd("sensors",sensors_cmd,"Lists IR sensors");
}


sensorIR_t*
sensorIR_new(int id) {
	sensorIR_t* this = (sensorIR_t*) malloc(sizeof(sensorIR_t));
	event_t* event = (event_t*) malloc(sizeof(event_t));
	sensorIR_init(this, id ,event);
	if (nsensors < MAXSENSORS) {
		sensors[nsensors++] = this;
	}
	return this;
}

void 
sensorIR_init(sensorIR_t* this, int id , event_t* event) {
	int i;
	observable_init((observable_t *) this);
	this->id = id;
	for (i = 0; i < 2; i++) {
		// Set the associated train line for that sensor
		this->GPIOlines[i] = (id * 2) + i;
		// Set the line as input
		pinMode(this->GPIOlines[i], INPUT);
		/*if (wiringPiISR (this->GPIOlines[i], INT_EDGE_RISING, sensorIR_isr[i]) < 0) {
		 fprintf (stderr, "Unable to setup ISR: %s\n", strerror (errno)) ;
		 }*/
	}
//	this->last_reading = -1;
    this->event = event;
    rt_mutex_create(&this->mutex, NULL);

}

void 
sensorIR_destroy(sensorIR_t* this) {
	free(this);
}

int 
sensorIR_readLine(sensorIR_t* this, int trainLine) //trainLine: 0 for diesel, 1 for renfe
{
	int r;

	// Adquire mutex
	rt_mutex_acquire(&(this->mutex), TM_INFINITE);

	// Read sensor line
	r = digitalRead(this->GPIOlines[trainLine]);

	// Release mutex
	rt_mutex_release(&this->mutex);
	return r;
}

void 
sensorIR_trainPassing(sensorIR_t* this) {
	int i, r;
	r = -1;

	// Adquire mutex
	rt_mutex_acquire(&(this->mutex), TM_INFINITE);
	this->event->flag = 0;
	for (i = 0; i < 2; i++) {
		// Read sensor line and check its state
		if (digitalRead(this->GPIOlines[i]) == HIGH) {
             // 4 if diesel is passing, 3 if renfe is passing, or 2 if no one                                
			r = i+3;
		}
	}
    if(r>2){
            
    this->event->flag = 1;
    this->event->passingTrain = r;
    }
	// Release mutex
	rt_mutex_release(&this->mutex);

	if (this->event->flag == 1) {
		observable_notify_observers((observable_t*) this);
		
	}
}

event_t* 
sensorIR_get_event (sensorIR_t* this){
                   return this->event;
}
