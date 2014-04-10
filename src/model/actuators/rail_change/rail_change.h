#ifndef RAIL_CHANGE_H
#define RAIL_CHANGE_H

#include "../actuator.h"

typedef enum {LEFT, RIGHT} direction_t;

typedef struct rail_change_t
{
    //Inheritance
    actuator_t actuator;

    //Attributes
	direction_t direction;
	pthread_mutex_t mutex;

} rail_change_t;


/*
 * Object creation / destruction
 */
rail_change_t* rail_change_new (int id);
void rail_change_init (rail_change_t* this, int id, direction_t direction);

/*
 * Getters / setters
 */
direction_t* rail_change_get_direction (rail_change_t* this);
void rail_change_set_direction (rail_change_t* this, direction_t direction);

/*
 * Observer function
 *
 * Rail change behaviour belongs here
 */
void rail_change_notify (observer_t* this, observable_t* observable);


#endif
