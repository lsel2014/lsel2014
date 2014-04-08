/*
 * sensorIR.h
 *
 *  Created on: 02/04/2014
 *      Author: Javier
 */

#ifndef SENSORIR_H
#define SENSORIR_H

#include <pthread.h>
#include "sensor.h"

#define NUMBER_OF_TRAINS 2 //esto habr�a que sacarlo de aqu� cuanto antes

struct sensorIR_t;

typedef struct sensorIR_t {

	struct sensor_t sensor;

	int GPIOlines[NUMBER_OF_TRAINS];
	int last_reading;

	RT_MUTEX mutex_sensorIR;

} sensorIR_t;

sensor_t* sensorIR_new (int id);
void sensorIR_init (sensor_t* this, int id);
void sensorIR_destroy (sensor_t* this);

int sensorIR_readLine(sensorIR_t* this, int trainLine);
int sensorIR_trainPassing(sensorIR_t* this);

void sensorIR_process_data (sensor_t* this);

#endif
