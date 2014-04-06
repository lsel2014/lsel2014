#ifndef crossing_gate_H
#define crossing_gate_H

#include "actuator.h"

typedef enum {UP, DOWN} position_t;

typedef struct crossing_gate_t
{
    //Inheritance
    actuator_t actuator;

    //Attributes
	position_t position;
	pthread_mutex_t mutex;

} crossing_gate_t;


/*
 * Object creation / destruction
 */
crossing_gate_t* crossing_gate_new (int id);
void crossing_gate_init (crossing_gate_t* this, int id, position_t position);

/*
 * Getters / setters
 */
position_t* crossing_gate_get_position (crossing_gate_t* this);
void crossing_gate_set_position (crossing_gate_t* this, position_t position);

/*
 * Observer function
 *
 * Crossing gate behaviour belongs here
 */
void crossing_gate_notify (observer_t* this, observable_t* observable);


#endif
