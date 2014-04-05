#include "actuator.h"
#include "interp.h"

actuator_t* actuator_new (sun_date_t date, direction_t initialRailway){

	actuator_t* this = (actuator_t*) malloc(sizeof(actuator_t));

	this->railChange = rail_change_new(initialRailway);
	this->trafficLight = traffic_light_new(OFF);
	this->sun = sun_new (date, 1, 1);
	this->frameGen = NULL;
	pthread_mutex_init (&this->mutex, NULL);

	interp_addcmd ("railway", rail_change_cmd, "Changes railway (left | right)");
	interp_addcmd ("sun", sun_cmd, "Starts a given day simulation (dd/mm/yyyy)");
	interp_addcmd ("speed", speed_cmd, "Changes selected train's speed");

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

/*
 * Commands
 */

int rail_change_cmd (char* arg){

	if (0 == strcmp (arg, "left"){
		rail_change_t* railChange = actuator_get_rail_change (actuator);
		rail_change_set_direction(railChange, LEFT);
		printf ("Left rail selected\n");
		return 0;
	}
	if (0 == strcmp (arg, "right"){
		rail_change_t* railChange = actuator_get_rail_change (actuator);
		rail_change_set_direction(railChange, RIGHT);
		printf ("Right rail selected\n");
		return 0;
	}
	return 1;
}

int sun_cmd (char* arg){
	sun_destroy (actuator->sun);
	sun_t* sun = sun_new (arg, 1, 1);
	actuator_set_sun (actuator, sun);
	printf ("Simulating day: %s\n", arg);
	return 0;
}

int speed_cmd (char* arg){
	// Cosas de la velocidad...
	return 0;
}


// Dummy main, just to test compilation and fix syntax errors

int main (int argc, char** argv){
	actuator_t* actuators = actuator_new("01/01/2001", RIGHT);
	interp_run();
	return 0;
}
	

