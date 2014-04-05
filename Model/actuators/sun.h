#ifndef SUN_H
#define SUN_H

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#include <wiringPiI2C.h>

typedef const char* sun_date_t;
typedef const char* sun_time_t;

#define sun_date_cmp strcmp


typedef struct sun_t {

	sun_date_t date;
	sun_time_t sunrise;
	sun_time_t sunset;

	pthread_mutex_t mutex;
} sun_t;

sun_t* sun_new (sun_date_t date, int prio, int ceil);
void sun_init (sun_t* this, sun_date_t date, int prio, int ceil);
void sun_set_time (sun_t* this, sun_date_t date);
int dateToInt (sun_t* this, int param, int riseSet);
//int sun_get_luminosity (sun_t* this);
void sendHour (sun_t* this);
void sun_destroy (sun_t* this);


#endif
