/*
 LSEL 2014

 @authors J. Martin
 @date April 2014

 Semaphore test
 
 */

#include "trafficLight.h"
#include "../../train.h"
#include "../../dcc.h"
#include "../observer.h"
#include <stdio.h>
#include <assert.h>

int main() {
	trafficLight_t* light;
	/*dcc_sender_t* dccobject = dcc_new(12, 50);
	 train_t* train = train_new("Diesel", 0b0000100, '0', 20, dccobject);
	 telemetry_t* telem1 = {'1', '20'};
	 telemetry_t* telem2 = {'2', '20'};
	 
	 rain_set_direction(train, FORWARD);*/

	// Create and add sensible sector
	light = trafficLight_new(0, 13);
	trafficLight_set_sensibleSector(light, '1', FORWARD);

	// Check initial state	
	assert(trafficLight_get_state(light) == OFF);
	/*
	 // Register train
	 observable_register_observer((observable_t*) train, (observer_t*) light);
	 
	 // Change train telem to match the sector. Check semaphore on.
	 train_set_telemetry(train, telem1);
	 assert (trafficLight_get_state(light) == ON);
	 
	 // Change train telem not to match the sector. Check semaphore off.
	 train_set_telemetry(train, telem2);
	 assert (trafficLight_get_state(light) == OFF);*/

	return 0;
}

