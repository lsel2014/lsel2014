#ifndef CROSSINGGATE_H
#define CROSSINGGATE_H


#include "actuator.h"
#include "../../train.h"

typedef enum {UP, DOWN} position_t;

typedef struct crossingGate_t
{
    //Inheritance
    actuator_t actuator;

    //Attributes
	position_t position;
	char sectorCrossing;

	//Private
	char trainsInside[MAXTRAINS];
	int trainsInsideIndex;

	RT_MUTEX mutex;

} crossingGate_t;


/*
 * Object creation / destruction
 */
crossingGate_t* crossingGate_new (int id, char sectorCrossing);
void crossingGate_init (crossingGate_t* this, int id, char sectorCrossing, position_t position);
void crossingGate_destroy (crossingGate_t* this);

/*
 * Getters / setters
 */
position_t crossingGate_get_position (crossingGate_t* this);
void crossingGate_set_position (crossingGate_t* this, position_t position);

char crossingGate_get_sectorCrossing (crossingGate_t* this);
void crossingGate_set_sectorCrossing (crossingGate_t* this, char sectorCrossing);

//Checks if a certain train is inside the crossing gate sector
int crossingGate_getIDIndex (crossingGate_t* this, char trainID);

/*
 * Observer function
 *
 * Crossing gate behaviour belongs here
 */
void crossingGate_notify (observer_t* this, observable_t* observable);


#endif
