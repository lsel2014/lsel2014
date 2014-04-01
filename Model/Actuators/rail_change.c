#include "actuator.h"

typedef enum {LEFT, RIGHT} direction_t;

typedef struct actuator_rail_change_t
{
	actuator_t actuator;
	direction_t direction;

} actuator_rail_change_t;


//----------------------------

void
actuator_rail_change_init (actuator_t* this, int id, direction_t direction)
{
	actuator_rail_change_t* rail_change = (actuator_rail_change_t*) this;
	this->id = id;
	rail_change->direction = direction;

}

actuator_t*
actuator_rail_change_new (int id, direction_t direction)
{
	actuator_t* this = (actuator_t*) malloc (sizeof (actuator_rail_change_t));
	actuator_rail_change_init (this, id, direction);

	return this;

}


direction
actuator_rail_change_get_direction ()
{

}

void
actuator_rail_change_set_direction (direction_t direction)
{

}
