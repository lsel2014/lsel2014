#include "actuator.h"

actuator_t* actuator_new (sun_date_t date, direction_t initialRailway){

	actuator_t* this = (actuator_t*) malloc(sizeof(actuator_t));

	this->railChange = rail_change_new(initialRailway);
	this->trafficLight = traffic_light_new(OFF);
	this->sun = sun_new (date, 1, 1);
	pthread_mutex_init (&this->mutex, NULL);
	//this->frameGen = NULL;
	return this;

}

/*
 * Setters
 */

void actuator_set_rail_change (actuator_t* this, rail_change_t* rail_change){
	pthread_mutex_lock (&this->mutex);
	this->railChange = rail_change;
	pthread_mutex_unlock (&this->mutex);
}
	
void actuator_set_traffic_light (actuator_t* this, traffic_light_t* traffic_light){
	pthread_mutex_lock (&this->mutex);
	this->trafficLight = traffic_light;
	pthread_mutex_unlock (&this->mutex);
}

void actuator_set_sun (actuator_t* this, sun_t* sun){
	pthread_mutex_lock (&this->mutex);
	this->sun = sun;
	pthread_mutex_unlock (&this->mutex);
}

void actuator_set_frame_gen (actuator_t* this, frames_t* frame_gen){
	pthread_mutex_lock (&this->mutex);
	this->frameGen = frame_gen;
	pthread_mutex_unlock (&this->mutex);
}

/*
 * Getters
 */

rail_change_t* actuator_get_rail_change (actuator_t* this){
	return this->railChange;
}

traffic_light_t* actuator_get_traffic_light (actuator_t* this){
	return this->trafficLight;
}

sun_t* actuator_get_sun (actuator_t* this){
	return this->sun;
}

frames_t* actuator_get_frame_gen (actuator_t* this){
	return this->frameGen;
}

// Dummy main, just to test compilation and fix syntax errors

int main (int argc, char** argv){
	actuator_t* actuators = actuator_new("01/01/2001", RIGHT);
	return 0;
}
	

