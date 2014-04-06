#ifndef SUN_H
#define SUN_H

#include "actuator.h"

#define sun_date_cmp strcmp


/*
 * Virtual time objects
 */
typedef const char* sun_date_t;
typedef struct sun_time_t
{
	char hours;
	char minutes;
	char seconds;

} sun_time_t;


/*
 * Object creation / destruction
 */
sun_time_t* sun_time_new (char hours, char minutes, char seconds);
void sun_time_init (sun_time_t* this, char hours, char minutes, char seconds);

/*
 * Getters / setters
 */
char sun_time_get_hours (sun_time_t* this);
char sun_time_get_minutes (sun_time_t* this);
char sun_time_get_seconds (sun_time_t* this);

void sun_time_set_hours (sun_time_t* this, char hours);
void sun_time_set_minutes (sun_time_t* this, char minutes);
void sun_time_set_seconds (sun_time_t* this, char seconds);


/*
 * Sun
 */
typedef struct sun_t
{
    //Inheritance
    actuator_t actuator;

    //Attributes
	sun_date_t date;
	sun_time_t sunrise;
	sun_time_t sunset;
	
	char i2c_address;
	int i2c_fd;
    
    int current_simulated_time;
	
	pthread_mutex_t mutex;

} sun_t;


/*
 * Object creation / destruction
 */
sun_t* sun_new (int id, sun_date_t date, char i2c_address, int prio, int ceil);
void sun_init (sun_t* this, int id, sun_date_t date, char i2c_address, int prio, int ceil);
void sun_destroy (sun_t* this);


/*
 * Getters / setters
 */
sun_date_t* sun_get_date (sun_t* this);
sun_time_t* sun_get_sunrise (sun_t* this);
sun_time_t* sun_get_sunset (sun_t* this);

char sun_get_i2c_address (sun_t* this);
int sun_get_i2c_fd (sun_t* this);

int sun_get_current_simulated_time (sun_t* this);

//--

void sun_set_date (sun_t* this, sun_date_t date);


/*
 * Communication with external uC
 */
void sun_update_simulated_time (sun_t* this);


/*
 * Observer function
 *
 * Sun behaviour belongs here
 */
void sun_notify (observer_t* this, observable_t* observable);


#endif
