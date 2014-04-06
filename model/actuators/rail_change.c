#include <stdlib.h>
#include <pthread.h>

#include "rail_change.h"


rail_change_t*
rail_change_new (int id)
{
	rail_change_t* this = (rail_change_t*) malloc (sizeof (rail_change_t));
	rail_change_init (this, id, LEFT);

	return this;
}

void
rail_change_init (rail_change_t* this, int id, direction_t direction)
{
    actuator_init ((actuator_t*) this, id, rail_change_notify);

	this->direction = direction;
	pthread_mutex_init (&this->mutex, NULL);
}


direction_t*
rail_change_get_direction (rail_change_t* this)
{
    return &this->direction;
}

void
rail_change_set_direction (rail_change_t* this, direction_t direction)
{
    pthread_mutex_lock (&this->mutex);
    this->direction = direction;
    pthread_mutex_unlock (&this->mutex);
}


void
rail_change_notify (observer_t* this, observable_t* observable)
{
    /*
     * RAIL CHANGE BEHAVIOUR GOES HERE
     */
}
