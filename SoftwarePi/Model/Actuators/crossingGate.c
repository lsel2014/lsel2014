#include <stdlib.h>
#include <pthread.h>
#include <native/mutex.h>
#include <stdbool.h>

#include "crossingGate.h"


crossingGate_t*
crossingGate_new (int id, char sectorCrossing)
{
	crossingGate_t* this = (crossingGate_t*) malloc (sizeof (crossingGate_t));
	crossingGate_init (this, id, sectorCrossing, DOWN);

	return this;
}

void
crossingGate_init (crossingGate_t* this, int id, char sectorCrossing, position_t position)
{
	int i;

    actuator_init ((actuator_t*) this, id, crossingGate_notify);

    this->sectorCrossing = sectorCrossing;
	this->position = position;

	for (i=0; i<MAXTRAINS; i++) {
		this->trainsInside[i] = 0;
	}
	this->trainsInsideIndex = 0;

	rt_mutex_create (&this->mutex, NULL);
}

void
crossingGate_destroy (crossingGate_t* this)
{
	//This should be done properly
	free(this);
}

position_t*
crossingGate_get_position (crossingGate_t* this)
{
    return this->position;
}

void
crossingGate_set_position (crossingGate_t* this, position_t position)
{
    rt_mutex_aquire (&this->mutex);
    this->position = position;
    rt_mutex_release (&this->mutex);
}

int
crossingGate_get_sectorCrossing (crossingGate_t* this)
{
	return this->sectorCrossing;
}

void
crossingGate_set_sectorCrossing (crossingGate_t* this, char sectorCrossing)
{
	rt_mutex_aquire (&this->mutex);
    this->sectorCrossing = sectorCrossing;
    rt_mutex_release (&this->mutex);
}

int
crossingGate_getIDIndex (crossingGate_t* this, char trainID)
{
	int r = -1;
	int i;

	for (i=0; i<MAXTRAINS; i++) {
		if (this->trainsInside[i] == trainID) {
			r = i;
		}
	}

	return r; //returns the array's index for that ID, or -1 if the ID was not in the list
}

void
crossingGate_notify (observer_t* this, observable_t* observable)
{
	train_t* train = (train_t*) observable;
	crossingGate_t* thisCG = (crossingGate_t*) this;
	telemetry_t telemetry = train->telemetry;
	char sector = telemetry->sector;

	//Check if a train has just entered the crossing gate sector
	if (sector == thisCG->sectorCrossing) {

		//Put that train inside the list, increment the index and lower the barrier
		thisCG->trainsInside[thisCG->trainsInsideIndex++] = train->ID;
		crossingGate_set_position (thisCG, DOWN);

	} else { //The train is not in the the crossing gate sector

		int position = crossingGate_getIDIndex (thisCG,train->ID);
		int i;

		//Check if the train ID is in the list
		//If so, it means the train has just left the crossing gate sector
		if (position >= 0) {

			//Erase that train ID, and fill the gap
			for (i = position; i<MAXTRAINS-1; i++) {
				thisCG->trainsInside[i] = thisCG->trainsInside[i+1];
			}

			//Decrement the index. If there are no trains left, rise the barrier
			if (--thisCG->trainsInsideIndex == 0)
				crossingGate_set_position (thisCG, UP);
		}
	}
}
