/*
	LSEL 2014

	@authors _____, J. Martin
	@date April 2014

	Represents a traffic semaphore that activates when some trains are in certain sectors with a certain direction.
	
*/


#include <stdbool.h>
#include "trafficLight.h"

#include "actuator.h"

#include <wiringPi.h>

static trafficLight_t* light;

void setup_trafficLight (void) {
	light = trafficLight_new (0, 13);
	
	trafficLight_set_sensibleSector(light, '1', FORWARD);
	trafficLight_set_sensibleSector(light, '3', REVERSE);
	
	interp_addcmd("semaphore", trafficLight_cmd, "Set semaphore state\n");
}

int trafficLight_cmd(char* arg) {
	if (0 == strcmp(arg, "list")) {
		printf("Semaphore\t%s\r\n", (light->state) == ON ? "ON" : "OFF");
		
		return 0;
	}
	if (0 == strncmp(arg, "set ", strlen("set "))) {
		int state;
		state = atoi(arg + strlen("set "));
		trafficLight_set_state (light, state==1 ? ON : OFF);
		return 1;
	}
	return 1;
}

void
trafficLight_init (trafficLight_t* this, int id, int GPIOline, state_t state)
{
	actuator_init ((actuator_t*) this, id, trafficLight_notify);
	
	this->state = state;
	this->GPIOline = GPIOline;
	this->activeTrainsCount = 0;
	this->sectors.sectorCount = 0;
	rt_mutex_create (&this->mutex, NULL);
	
	// Set GPIO line as output
	pinMode (this->GPIOline, OUTPUT);
	trafficLight_set_state (this, state);
	
}

trafficLight_t*
trafficLight_new (int id, int GPIOline)
{
	trafficLight_t* this = (trafficLight_t*) malloc (sizeof (trafficLight_t));
	trafficLight_init (this, id, GPIOline, OFF);

	return this;

}

void 
trafficLight_set_sensibleSector (trafficLight_t* this, char sector, train_direction_t direction)
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
trafficLight_set_state (trafficLight_t* this, state_t state)
{
	rt_mutex_acquire(&(this->mutex), TM_INFINITE);
	this->state = state;
	if (state == ON) 
		digitalWrite (this->GPIOline, 1);
	else
		digitalWrite (this->GPIOline, 0);
	
	rt_mutex_release(&(this->mutex));

}

void
trafficLight_notify (observer_t* this, observable_t* observable)
{
    train_t *tren = (train_t *) observable;
    char sector = tren->telemetry->sector;
    train_direction_t direction = tren->direction;
    int i;
    int inside = -1;
    trafficLight_t* me = (trafficLight_t* ) this;

    // Check if the train is in the active train list
    for (i=0;i<(me->activeTrainsCount);i++)
    {
    	if ((me->activeTrains[i]) == (tren->ID))
    	{
    		inside=i;
    	}
    }
    
    // If it is inside
    if (inside>=0)
    {
    	// Check if it still has to be active
    	bool keep=false;
    	for (i=0; i<(me->sectors.sectorCount); i++)
    	{
    		if (sector == (me->sectors.sectors[i]) && direction == (me->sectors.directions[i]))
    		{
    			keep = true;
    		}
    	}
    	
    	// If not, remove it from the array, by moving the following ellements.
    	if (keep == false)
    	{
    		for (i=inside; i<(me->activeTrainsCount-1);i++)
    		{
    			me->activeTrains[i] = me->activeTrains[i+1];
    		}
    		me->activeTrainsCount--;
    		
    		// If there are no active trains, turn off the lights.
    		if (me->activeTrainsCount == 0) 
    			trafficLight_set_state (me, OFF);
    	}
    }
    // If it was not in the list...
    else
    {
    	// Check if it has to be active
    	bool keep=false;
    	for (i=0; i<(me->sectors.sectorCount);i++)
    	{
    		if (sector == me->sectors.sectors[i] && direction == me->sectors.directions[i])
    		{
    			keep = true;
    		}
    	}
    	
    	// If it has to be active, add to the active trains
    	if (keep == true)
    	{
    		me->activeTrains[me->activeTrainsCount] = tren->ID;
    		me->activeTrainsCount++;
    		
    		if (me->activeTrainsCount == 1) 
    			trafficLight_set_state (me, ON);
    	}
    }			
    
    
}


