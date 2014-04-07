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
#include <wiringPi.h>

/*
#define COLOR_RED "\x1b[31m"
#define COLOR_GREEN "\x1b[32m"
#define COLOR_RESET "\x1b[0m"


static void sensorIR_isr0 (void)
{
	rt_printf(COLOR_GREEN"Detectado diesel\n"COLOR_RESET);
	//detected diesel train, do something
}

static void sensorIR_isr1 (void)
{
	rt_printf(COLOR_GREEN"Detectado renfe\n"COLOR_RESET);
	//detected renfe train, do something
}



static void (*sensorIR_isr[]) (void) = {
  sensorIR_isr0,
  sensorIR_isr1,
};

*/
sensorIR_t*
sensorIR_new(int id, int ceil)
{
	sensorIR_t* this = (sensorIR_t*) malloc(sizeof(sensorIR_t));
	sensorIR_init(this, id, ceil);
	return this;
}

void
sensorIR_init(sensorIR_t* this, int id, int ceil)
{
	int i;

	this->id = id;

	for (i = 0; i < NUMBER_OF_TRAINS; i++) {
		// Set the associated train line for that sensor
		this->GPIOlines[i] = (id * 2) + i;
		
		// Set the line as input
		pinMode (this->GPIOlines[i], INPUT);

		/*if (wiringPiISR (this->GPIOlines[i], INT_EDGE_RISING, sensorIR_isr[i]) < 0) {
		    fprintf (stderr, "Unable to setup ISR: %s\n", strerror (errno)) ;
		}*/
	}
	
	// Initialize mutex
	pthread_mutex_init(&this->mutex_sensorIR, NULL);
}

void
sensorIR_destroy(sensorIR_t* this)
{
	if (this->id)
		free((int*) this->id);
	if (this->GPIOlines)
		free((int*) this->GPIOlines);
	pthread_mutex_destroy(&this->mutex_sensorIR);
	free(this);
}

int
sensorIR_get_id(sensorIR_t* this)
{
	return this->id;
}

void
sensorIR_set_id(sensorIR_t* this, int newid)
{
	this->id = newid;
}

int
sensorIR_readLine(sensorIR_t* this, int trainLine) //train: 0 for diesel, 1 for renfe
{
	int r;
	
	// Adquire mutex
	pthread_mutex_lock(&this->mutex_sensorIR);
	
	// Read sensor line
	r = digitalRead(this->GPIOlines[trainLine]);
	
	// Release mutex
	pthread_mutex_unlock(&this->mutex_sensorIR);
	return r;
}

int
sensorIR_trainPassing(sensorIR_t* this)
{
	int i, r;
	r=-1;

	// Adquire mutex
	pthread_mutex_lock(&this->mutex_sensorIR);
	
	for (i = 0; i < NUMBER_OF_TRAINS; i++) {
		// Read sensor line and check its state
		if (digitalRead(this->GPIOlines[i]) == HIGH) {
			r=i;
		}
	}
	
	// Release mutex
	pthread_mutex_unlock(&this->mutex_sensorIR);

	return r; //returns 0 if diesel is passing, 1 if renfe is passing, or -1 if no one
}

