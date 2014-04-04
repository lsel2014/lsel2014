#ifndef TRAFFIC_LIGHT_H
#define TRAFFIC_LIGHT_H
#include <stdlib.h>
#include <pthread.h>

typedef enum {ON, OFF} state_t;

typedef struct traffic_light_t
{
	state_t state;

	pthread_mutex_t mutex;

} traffic_light_t;


//----------------------------

void traffic_light_init (traffic_light_t* this, state_t state);

traffic_light_t* traffic_light_new (state_t state);


state_t traffic_light_get_state (traffic_light_t* this);

void traffic_light_set (traffic_light_t* this, state_t state);

#endif
