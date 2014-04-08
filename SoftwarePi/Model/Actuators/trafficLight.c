#include "trafficLight.h"

void
trafficLight_init (trafficLight_t* this, state_t state)
{
	this->state = state;
	pthread_mutex_init (&this->mutex, NULL);

}

trafficLight_t*
trafficLight_new (state_t state)
{
	trafficLight_t* this = (trafficLight_t*) malloc (sizeof (trafficLight_t));
	trafficLight_init (this, state);

	return this;

}


state_t
trafficLight_get_state (trafficLight_t* this)
{

return this->state;

}


void
trafficLight_set (trafficLight_t* this, state_t state)
{
pthread_mutex_lock (&this->mutex);
this->state = state;
pthread_mutex_unlock (&this->mutex);

}
