/*
	LSEL 2014

	@authors _____, J. Martin
	@date April 2014

	Represents a electrical rail changer.
	
*/

#include "railChange.h"


void
rail_change_init (rail_change_t* this, direction_t direction)
{
	this->direction = direction;
	
	rt_mutex_create (&this->mutex, NULL);

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
	rt_mutex_acquire(&(this->mutex), TM_INFINITE);
	
	this->direction = direction;
	
	rt_mutex_release(&(this->mutex));
}
