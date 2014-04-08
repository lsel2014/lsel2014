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
	
	char sectors[MAXSENSIBLESECTORS];
	
	train_direction_t [MAXSENSIBLESECTORS];
} sensibleSectors_t;


typedef enum {ON, OFF} state_t;

typedef struct trafficLight_t
{
	//Inheritance
    actuator_t actuator;
    
    int GPIOline;
    
	state_t state;
	
	sensibleSectors_t sectors;
	
	int activeTrainsCount;
	char activeTrains[MAXTRAINS];

	RT_MUTEX mutex;

} trafficLight_t;


/*
 * Object creation / destruction
 */
void traffic_light_init (traffic_light_t* this, int id, int GPIOline, state_t state);

trafficLight_t* trafficLight_new (int id, int GPIOline);

/*
 * Getters / setters
 */
state_t trafficLight_get_state (trafficLight_t* this);
void trafficLight_set_sensibleSector (trafficLight_t* this, char sector, train_direction_t sentido);
void trafficLight_set_state (trafficLight_t* this, state_t state);

/*
 * Observer function
 *
 * Traffic lights behaviour belongs here
 */
void traffic_light_notify (observer_t* this, observable_t* observable);

#endif
