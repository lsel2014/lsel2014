#include "traffic_light.h"

void
traffic_light_init (traffic_light_t* this, state_t state)
{
	this->state = state;
	pthread_mutex_init (&this->mutex, NULL);

}

traffic_light_t*
traffic_light_new (state_t state)
{
	traffic_light_t* this = (traffic_light_t*) malloc (sizeof (traffic_light_t));
	rail_change_init (this, state);

	return this;

}


state_t
traffic_light_get_state (traffic_light_t* this)
{

return this->state;

}


void
traffic_light_set (traffic_light_t* this, state_t state)
{
pthread_mutex_lock (&this->mutex);
this->state = state;
pthread_mutex_unlock (&this->mutex);

}
