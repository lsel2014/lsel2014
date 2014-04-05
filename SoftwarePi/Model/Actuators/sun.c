#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <wiringPiI2C.h>
#include "sun.h"

sun_t*
sun_new (sun_date_t date,char i2c_address, int prio, int ceil)
{
	sun_t* this = (sun_t*) malloc (sizeof (sun_t));
	sun_init (this, date,i2c_address, prio, ceil);
	return this;
}

void
sun_init (sun_t* this, sun_date_t date, char i2c_address,int prio, int ceil)
{
	this->sunrise = NULL;
	this->sunset = NULL; 
	this->i2c_address= i2c_address;
	this->i2c_fd = wiringPiI2CSetup(i2c_address);
	sun_set_date (this, date);
	pthread_mutex_init (&this->mutex, NULL);
}

void
sun_set_date (sun_t* this, sun_date_t date)
{

	pthread_mutex_lock (&this->mutex);
	this->date = date;
	pthread_mutex_unlock (&this->mutex);

	//if (sun_date_cmp (date, current_date) != 0) {
	char cmd[1024];
	char aux[6];
	char *unparsedDate = strdup(this->date);
	char *day = strtok(unparsedDate, "/");
	char *month = strtok(NULL, "/");
	snprintf (aux, 6, "%s/%s", day, month);
	snprintf (cmd, 1024,
			"wget -q --output-document=sunrise.xml "
			"http://www.earthtools.org/sun/40.4521/-3.7266/%s/1/0", aux);
	system(cmd);
	sun_parse_data (this);
	wiringPiI2CWrite(this->i2c_fd, 0xFF);
	wiringPiI2CWrite(this->i2c_fd, 0x01);
	wiringPiI2CWrite(this->i2c_fd, this->sunrise.hours);
	wiringPiI2CWrite(this->i2c_fd, this->sunrise.minutes);
	wiringPiI2CWrite(this->i2c_fd, this->sunrise.seconds);
	wiringPiI2CWrite(this->i2c_fd, this->sunset.hours);
	wiringPiI2CWrite(this->i2c_fd, this->sunset.minutes);
	wiringPiI2CWrite(this->i2c_fd, this->sunset.seconds);
}


void sun_update_simulated_time(sun_t* this){
	wiringPiI2CWrite(this->i2c_fd, 0xFF);
	wiringPiI2CWrite(this->i2c_fd, 0x01);
	this->current_simulated_time = wiringPiI2CRead(this->i2c_fd)<<8 +wiringPiI2CRead(this->i2c_fd);
}

int sun_get_simulated_time(sun_t* this){
	return this->current_simulated_time;
};

void
sun_destroy (sun_t* this)
{	if(this->date) free ((sun_date_t*)this->date);
	if (this->sunrise) free ((sun_time_t*)this->sunrise);
	if (this->sunset) free ((sun_time_t*)this->sunset);
	free (this);
}
