#ifndef TRAFFIC_LIGHT_H
#define TRAFFIC_LIGHT_H
#include <stdlib.h>
#include <pthread.h>

#include <native/mutex.h>
#include <actuator.h>

#define MAXTRAINS 10

#define MAXSENSIBLESECTORS 2

typedef struct sensibleSectors_t
{
	int sectorCount;
	
	int sectors[MAXSENSIBLESECTORS];
	
	bool direction [MAXSENSIBLESECTORS];
} sensibleSectors_t;


typedef enum {ON, OFF} state_t;

typedef struct trafficLight_t
{
	//Inheritance
    actuator_t actuator;
    
	state_t state;
	
	sensibleSectors_t sectors;
	
	int activeTrainsCount;
	int activeTrains[MAXTRAINS];

	RT_MUTEX mutex;

} trafficLight_t;


/*
 * Object creation / destruction
 */
void traffic_light_init (traffic_light_t* this, int id, state_t state);

trafficLight_t* trafficLight_new (int id);

/*
 * Getters / setters
 */
state_t trafficLight_get_state (trafficLight_t* this);
void trafficLight_set_sensibleSector (trafficLight_t* this, int sector, bool sentido);
void trafficLight_set_state (trafficLight_t* this, state_t state);

/*
 * Observer function
 *
 * Traffic lights behaviour belongs here
 */
void traffic_light_notify (observer_t* this, observable_t* observable);

#endif
