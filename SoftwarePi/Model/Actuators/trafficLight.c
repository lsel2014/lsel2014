#include <stdbool.h>
#include "trafficLight.h"

#include <native/mutex.h>


void
trafficLight_init (trafficLight_t* this, int id, state_t state)
{
	actuator_init ((actuator_t*) this, id, traffic_light_notify);
	
	this->state = state;
	this->activeTrainsCount = 0;
	this->sectors.sectorCount = 0;
	rt_mutex_create (&this->mutex, NULL);
	
}

trafficLight_t*
trafficLight_new (int id)
{
	trafficLight_t* this = (trafficLight_t*) malloc (sizeof (trafficLight_t));
	trafficLight_init (this, id, OFF);

	return this;

}

void 
trafficLight_set_sensibleSector (trafficLight_t* this, int sector, bool direction)
{
	if (this->sectors.sectorCount == MAXSENSIBLESECTORS)
		return;
	
	this->sectors.sectors[this->sectors.sectorCount] = sector;
	this->sectors.sectors[this->sectors.sectorCount] = direction;
	this->sectors.sectorCount++;

}
state_t
trafficLight_get_state (trafficLight_t* this)
{

return this->state;

}


void
trafficLight_set (trafficLight_t* this, state_t state)
{
	rt_mutex_acquire(&(this->mutex), TM_INFINITE);
	this->state = state;
	pthread_mutex_unlock (&this->mutex);
	rt_mutex_release(&(this->mutex));

}

void
traffic_light_notify (observer_t* this, observable_t* observable)
{
    /*
     * TRAFFIC LIGHTS BEHAVIOUR GOES HERE
     */
}


