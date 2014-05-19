/*
 LSEL 2014

 @authors _____, J. Martin

 */
#ifndef TRAFFIC_LIGHT_H
#define TRAFFIC_LIGHT_H

#include <stdlib.h>
#include <native/mutex.h>

#include "train.h"
#include "interp.h"
#include "observer.h"

#define MAXSENSIBLESECTORS 4
#define MAXTRAFFICLIGHTS 4

typedef enum {
	ON, OFF
} state_t;

typedef struct trafficLight_t {
	observable_t observable;
	
	int GPIOline;
	state_t state;

	RT_MUTEX mutex;
} trafficLight_t;

extern trafficLight_t* trafficLights[MAXTRAFFICLIGHTS];

void trafficLight_setup(void);
int trafficLight_cmd(char* arg);
/*
 * Object creation / destruction
 */
void trafficLight_init(trafficLight_t* this, int id, int GPIOline,
		state_t state);

trafficLight_t* trafficLight_new(int id, int GPIOline);

/*
 * Getters / setters
 */
state_t trafficLight_get_state(trafficLight_t* this);
void trafficLight_set_sensibleSector(trafficLight_t* this, int sector,
		train_direction_t sentido);
void trafficLight_set_state(trafficLight_t* this, state_t state);

/*
 * Observer function
 *
 * Traffic lights behaviour belongs here
 */
void trafficLight_notify(observer_t* this);

#endif
