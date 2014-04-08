#ifndef TRAFFIC_LIGHT_H
#define TRAFFIC_LIGHT_H
#include <stdlib.h>
#include <pthread.h>

typedef enum {ON, OFF} state_t;

typedef struct trafficLight_t
{
	state_t state;

	pthread_mutex_t mutex;

} trafficLight_t;


//----------------------------

void trafficLight_init (trafficLight_t* this, state_t state);

trafficLight_t* trafficLight_new (state_t state);


state_t trafficLight_get_state (trafficLight_t* this);

void trafficLight_set (trafficLight_t* this, state_t state);

#endif
