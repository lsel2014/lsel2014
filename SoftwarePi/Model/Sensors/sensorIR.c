/*
 * sensorIR.c
 *
 *  Created on: 02/04/2014
 *      Author: Javier
 */

#include "sensorIR.h"

#include <pthread.h>
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

sensor_t*
sensorIR_new(int id)
{
	sensor_t* this = (sensor_t*) malloc (sizeof(sensorIR_t));
	sensorIR_init(this, id);
	return this;
}

void
sensorIR_init(sensor_t* this, int id)
{
	int i;
	sensorIR_t* thisIR = (sensorIR_t*) this;

	sensor_init(this, sensorIR_process_data, id);

	for (i = 0; i < NUMBER_OF_TRAINS; i++) {
		// Set the associated train line for that sensor
		thisIR->GPIOlines[i] = (id * NUMBER_OF_TRAINS) + i;

		// Set the line as input
		pinMode (thisIR->GPIOlines[i], INPUT);

		/*if (wiringPiISR (this->GPIOlines[i], INT_EDGE_RISING, sensorIR_isr[i]) < 0) {
			fprintf (stderr, "Unable to setup ISR: %s\n", strerror (errno)) ;
		}*/
	}

	thisIR->last_reading=-1;

	rt_mutex_create (&thisIR->mutex, NULL);

}

void
sensorIR_destroy(sensor_t* this)
{
	sensorIR_t* thisIR = (sensorIR_t*) this;

	// TODO -> REPAIR
	//free((observable_t*) this->observable);
	if (this->id)
		free((int*) this->id);
	if (thisIR->GPIOlines)
		free((int*) thisIR->GPIOlines);
	if (thisIR->last_reading)
			free((int*) thisIR->last_reading);
	//pthread_mutex_destroy(&thisIR->mutex_sensorIR);
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
sensorIR_trainPassing(sensorIR_t* this)
{
	int i, r;
	r=-1;

	// Adquire mutex
	rt_mutex_acquire(&(this->mutex), TM_INFINITE);

	for (i = 0; i < NUMBER_OF_TRAINS; i++) {
			// Read sensor line and check its state
			if (digitalRead(this->GPIOlines[i]) == HIGH) {
				r=i;
			}
	}

	this->last_reading = r; // 0 if diesel is passing, 1 if renfe is passing, or -1 if no one

	// Release mutex
	rt_mutex_release(&this->mutex);

	if (this->last_reading >= 0) {
		sensorIR_process_data ((sensor_t*)this);
	}
}

void
sensorIR_process_data (sensor_t* this)
{
	observable_notify_observers ((observable_t*) this);
}

