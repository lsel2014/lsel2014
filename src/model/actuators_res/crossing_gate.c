#include <stdlib.h>
#include <pthread.h>

#include "crossing_gate.h"


crossing_gate_t*
crossing_gate_new (int id)
{
	crossing_gate_t* this = (crossing_gate_t*) malloc (sizeof (crossing_gate_t));
	crossing_gate_init (this, id, DOWN);

	return this;
}

void
crossing_gate_init (crossing_gate_t* this, int id, position_t position)
{
    actuator_init ((actuator_t*) this, id, crossing_gate_notify);

	this->position = position;
	pthread_mutex_init (&this->mutex, NULL);
}


position_t*
crossing_gate_get_position (crossing_gate_t* this)
{
    return &this->position;
}

void
crossing_gate_set_position (crossing_gate_t* this, position_t position)
{
    pthread_mutex_lock (&this->mutex);
    this->position = position;
    pthread_mutex_unlock (&this->mutex);
}


void
crossing_gate_notify (observer_t* this, observable_t* observable)
{
    /*
     * CROSSING GATE BEHAVIOUR GOES HERE
     */
}
