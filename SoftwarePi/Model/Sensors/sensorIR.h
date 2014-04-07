/*
 * sensorIR.h
 *
 *  Created on: 02/04/2014
 *      Author: Javier
 */

#ifndef SENSORIR_H
#define SENSORIR_H

#include <pthread.h>

#define NUMBER_OF_TRAINS 2 //esto habría que sacarlo de aquí cuanto antes

struct sensorIR_t;

typedef struct sensorIR_t {

	int id;
	int GPIOlines[NUMBER_OF_TRAINS];

	pthread_mutex_t mutex_sensorIR;

} sensorIR_t;

sensorIR_t* sensorIR_new (int id, int ceil);
void sensorIR_init (sensorIR_t* this, int id, int ceil);
void sensorIR_destroy (sensorIR_t* this);

int sensorIR_get_id (sensorIR_t* this);
void sensorIR_set_id (sensorIR_t* this, int newid);

int sensorIR_readLine(sensorIR_t* this, int trainLine);
int sensorIR_trainPassing(sensorIR_t* this);

#endif
