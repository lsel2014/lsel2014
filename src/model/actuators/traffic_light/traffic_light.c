#include <stdlib.h>
#include <pthread.h>

#include "traffic_light.h"


traffic_light_t*
traffic_light_new (int id)
{
	traffic_light_t* this = (traffic_light_t*) malloc (sizeof (traffic_light_t));
	traffic_light_init (this, id, OFF);

	return this;
}

void
traffic_light_init (traffic_light_t* this, int id, state_t state)
{
    actuator_init ((actuator_t*) this, id, traffic_light_notify);

	this->state = state;
	pthread_mutex_init (&this->mutex, NULL);
}


state_t*
traffic_light_get_state (traffic_light_t* this)
{
    return &this->state;
}

void
traffic_light_set_state (traffic_light_t* this, state_t state)
{
    pthread_mutex_lock (&this->mutex);
    this->state = state;
    pthread_mutex_unlock (&this->mutex);
}


void
traffic_light_notify (observer_t* this, observable_t* observable)
{
    /*
     * TRAFFIC LIGHTS BEHAVIOUR GOES HERE
     */
}
