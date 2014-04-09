/*
 * sensorIR.h
 *
 *  Created on: 02/04/2014
 *      Author: Javier
 */

#ifndef SENSORIR_H
#define SENSORIR_H

#include "../observer.h"
#include <native/mutex.h>
#include "../../tasks.h"
#include "../../train.h"
#include "../../Interpreter/interp.h"
#define MAXSENSORS 4

#define IR_DEADLINE 20000000
#define IR_PERIOD 100000000

struct sensorIR_t;

typedef struct sensorIR_t {

	observable_t observable;
	int id;
	int GPIOlines[MAXTRAINS];
	int last_reading;

	RT_MUTEX mutex;

} sensorIR_t;

extern sensorIR_t* sensors[MAXSENSORS];
extern int nsensors;

void IRsensors_setup(void);
void IRsensors_poll (void* arg);
int sensors_cmd(char*arg);
sensorIR_t* sensorIR_new (int id);
void sensorIR_init (sensorIR_t* this, int id);
void sensorIR_destroy (sensorIR_t* this);

int sensorIR_readLine(sensorIR_t* this, int trainLine);
void sensorIR_trainPassing(sensorIR_t* this);

void sensorIR_process_data (sensorIR_t* this);

#endif
