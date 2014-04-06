#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <pthread.h>
#include <wiringPiI2C.h>

#include "sun.h"
#include "sunparse.h"

/*
 * Virtual time objects
 */

/*
 * Object creation / destruction
 */
sun_time_t*
sun_time_new (char hours, char minutes, char seconds)
{
    sun_time_t* this = (sun_time_t*) malloc (sizeof (sun_time_t));
    sun_time_init (this, hours, minutes, seconds);

    return this;
}

void
sun_time_init (sun_time_t* this, char hours, char minutes, char seconds)
{
    this->hours = hours;
    this->minutes = minutes;
    this->seconds = seconds;
}


/*
 * Getters / setters
 */
char
sun_time_get_hours (sun_time_t* this)
{
    return this->hours;
}

char
sun_time_get_minutes (sun_time_t* this)
{
    return this->minutes;
}

char
sun_time_get_seconds (sun_time_t* this)
{
    return this->seconds;
}


void
sun_time_set_hours (sun_time_t* this, char hours)
{
    this->hours = hours;
}

void
sun_time_set_minutes (sun_time_t* this, char minutes)
{
    this->minutes = minutes;
}

void
sun_time_set_seconds (sun_time_t* this, char seconds)
{
    this->seconds = seconds;
}



/*
 * Sun
 */

/*
 * Object creation / destruction
 */
sun_t*
sun_new (int id, sun_date_t date, char i2c_address, int prio, int ceil)
{
	sun_t* this = (sun_t*) malloc (sizeof (sun_t));
	sun_init (this, id, date, i2c_address, prio, ceil);

	return this;
}

void
sun_init (sun_t* this, int id, sun_date_t date, char i2c_address, int prio, int ceil)
{
    actuator_init ((actuator_t*) this, id, sun_notify); 

	this->sunrise = NULL;
	this->sunset = NULL;

	this->i2c_address = i2c_address;
	this->i2c_fd = wiringPiI2CSetup (i2c_address);

	sun_set_date (this, date);

	pthread_mutex_init (&this->mutex, NULL);
}

void
sun_destroy (sun_t* this)
{	
    if (this->actuator) free ((actuator_t*) this->actuator);
    if (this->date) free ((sun_date_t*) this->date);
	if (this->sunrise) free ((sun_time_t*) this->sunrise);
	if (this->sunset) free ((sun_time_t*) this->sunset);
	free (this);
}


/*
 * Getters / setters
 */
sun_date_t*
sun_get_date (sun_t* this)
{
    return &this->date;
}

sun_time_t*
sun_get_sunrise (sun_t* this)
{
    return &this->sunrise;
}

sun_time_t*
sun_get_sunset (sun_t* this)
{
    return &this->sunset;
}


char
sun_get_i2c_address (sun_t* this)
{
    return this->i2c_address;
}

int
sun_get_i2c_fd (sun_t* this)
{
    return this->i2c_fd;
}


int
sun_get_current_simulated_time (sun_t* this)
{
    return this->current_simulated_time;
}

//--

void
sun_set_date (sun_t* this, sun_date_t date)
{
	pthread_mutex_lock (&this->mutex);
	this->date = date;
	pthread_mutex_unlock (&this->mutex);

    /*
     * Command setup and specific daylight information downloading
     */
	char cmd[1024];
	char day_and_month[6];

	char* unparsedDate = strdup(this->date);
	char* day = strtok(unparsedDate, "/");
	char* month = strtok(NULL, "/");

	snprintf (day_and_month, 6, "%s/%s", day, month);
	snprintf (cmd, 1024,
			"wget -q --output-document=sunrise.xml "
			"http://www.earthtools.org/sun/40.4521/-3.7266/%s/1/0", day_and_month);

    //Execute wget
	system(cmd);
	
	/*
	 * Daylight information parsing and setting
	 */
	sun_read_sunrise_sunset (this);

    /*
     * Send data to sun uController
     */
    sun_send_sunrise_sunset (this);
}



/*
 * Communication with external uC
 */
void
sun_send_sunrise_sunset (sun_t* this)
{
	wiringPiI2CWrite (this->i2c_fd, 0xFF);
	wiringPiI2CWrite (this->i2c_fd, 0x01);

	wiringPiI2CWrite (this->i2c_fd, this->sunrise.hours);
	wiringPiI2CWrite (this->i2c_fd, this->sunrise.minutes);
	wiringPiI2CWrite (this->i2c_fd, this->sunrise.seconds);

	wiringPiI2CWrite (this->i2c_fd, this->sunset.hours);
	wiringPiI2CWrite (this->i2c_fd, this->sunset.minutes);
	wiringPiI2CWrite (this->i2c_fd, this->sunset.seconds); 
}

void
sun_update_simulated_time (sun_t* this)
{
	wiringPiI2CWrite (this->i2c_fd, 0xFF);
	wiringPiI2CWrite (this->i2c_fd, 0x01);

	this->current_simulated_time = wiringPiI2CRead (this->i2c_fd) << 8
	                               + wiringPiI2CRead (this->i2c_fd);
}



/*
 * Observer function
 */
void
sun_notify (observer_t* this, observable_t* observable)
{
    /*
     * SUN BEHAVIOUR GOES HERE
     */
}

