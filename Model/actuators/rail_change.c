#include "rail_change.h"

void
rail_change_init (rail_change_t* this, direction_t direction)
{
	this->direction = direction;
	pthread_mutex_init (&this->mutex, NULL);

}

rail_change_t*
rail_change_new (direction_t direction)
{
	rail_change_t* this = (rail_change_t*) malloc (sizeof (rail_change_t));
	rail_change_init (this, direction);

	return this;

}


direction_t
rail_change_get_direction (rail_change_t* this)
{
return this->direction;

}


void
rail_change_set_direction (rail_change_t* this, direction_t direction)
{
pthread_mutex_lock (&this->mutex);
this->direction = direction;
pthread_mutex_unlock (&this->mutex);

}
