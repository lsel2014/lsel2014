#ifndef ACTUATOR_H
#define ACTUATOR_H
#include "rail_change.h"
#include "traffic_light.h"
#include "sun.h"
#include "frames_gen.h"
#include <stdlib.h>

typedef struct actuator_t
{	

	rail_change_t* railChange;
	traffic_light_t* trafficLight;
	sun_t* sun;
	frames_t* frameGen;

	pthread_mutex_t mutex;

} actuator_t;


/*
 * Actuators init and creation
 */

actuator_t* actuator_new (sun_date_t date, direction_t initialRailway);

/*
 * Setters
 */

void actuator_set_rail_change (actuator_t* this, rail_change_t* rail_change);
void actuator_set_traffic_light (actuator_t* this, traffic_light_t* traffic_light);
void actuator_set_sun (actuator_t* this, sun_t* sun);
void actuator_set_frame_gen (actuator_t* this, frames_t* frame_gen);

/*
 * Getters
 */ 

rail_change_t* actuator_get_rail_change (actuator_t* this);
traffic_light_t* actuator_get_traffic_light (actuator_t* this);
sun_t* actuator_get_sun (actuator_t* this);
frames_t* actuator_get_frame_gen (actuator_t* this);

#endif
