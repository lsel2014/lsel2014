#ifndef TRAFFIC_LIGHT_H
#define TRAFFIC_LIGHT_H

#include "actuator.h"

typedef enum {ON, OFF} state_t;

typedef struct traffic_light_t
{
    //Inheritance
    actuator_t actuator;

    //Attributes
	state_t state;
	pthread_mutex_t mutex;

} traffic_light_t;


/*
 * Object creation / destruction
 */
traffic_light_t* traffic_light_new (int id);
void traffic_light_init (traffic_light_t* this, int id, state_t state);

/*
 * Getters / setters
 */
state_t* traffic_light_get_state (traffic_light_t* this);
void traffic_light_set_state (traffic_light_t* this, state_t state);

/*
 * Observer function
 *
 * Traffic lights behaviour belongs here
 */
void traffic_light_notify (observer_t* this, observable_t* observable);


#endif
