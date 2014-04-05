#ifndef RAIL_CHANGE_H
#define RAIL_CHANGE_H
#include <stdlib.h>
#include <pthread.h>

typedef enum {LEFT, RIGHT} direction_t;

typedef struct rail_change_t
{
	direction_t direction;

	pthread_mutex_t mutex;

} rail_change_t;


//----------------------------

void rail_change_init (rail_change_t* this, direction_t direction);

rail_change_t* rail_change_new (direction_t direction);


direction_t rail_change_get_direction ();

void rail_change_set_direction (rail_change_t* this, direction_t direction);

#endif
