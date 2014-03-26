#include "sun.h"
#include "sunparse.h"

//static pthread_t t_sun;

//static void* sun_advance_time (void* arg)
//{
// sun_t* this = (sun_t*) arg;
// struct timeval next_activation;
// struct timeval now, timeout;
// struct timeval period = { 0, 100000 };
//
// gettimeofday (&next_activation, NULL);
// while (1) {
// gettimeofday (&now, NULL);
// timersub (&next_activation, &now, &timeout);
// //timersub (&timeout, &next_activation, &now);
// select (0, NULL, NULL, NULL, &timeout) ;
// //timeradd (&next_activation, &next_activation, &period);
// timeradd (&next_activation, &period, &next_activation);
//
// pthread_mutex_lock (&this->mutex);
// sun_date_t date = this->date;
// // Cambiar
// sun_time_t time = this->time;
// pthread_mutex_lock (&this->mutex);
//
// sun_set_time (this, date, time);
// }
// return NULL;
//}


sun_t*
sun_new (sun_date_t date, sun_time_t time, int prio, int ceil)
{
	sun_t* this = (sun_t*) malloc (sizeof (sun_t));
	sun_init (this, date, time, prio, ceil);
	return this;
}

void
sun_init (sun_t* this, sun_date_t date, sun_time_t time, int prio, int ceil)
{
	this->sunrise = NULL;
	this->sunset = NULL; 
	sun_set_time (this, date, time);
	pthread_mutex_init (&this->mutex, NULL);
	//pthread_create (&t_sun, NULL, &sun_advance_time, this);
	//RT_MUTEX_CREATE (&this->mutex, ceil);
	//RT_TASK_CREATE (&this->tid, sun_advance_time, this, 1000, prio, 1024);
}

void
sun_set_time (sun_t* this, sun_date_t date, sun_time_t time)
{

	pthread_mutex_lock (&this->mutex);
	this->date = date;
	this->time = time;
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
	//f = popen (cmd, "r");
	sun_parse_data (this);
	//}
	pthread_mutex_lock (&this->mutex);
	// Cambiarlo
	//this->luminosity = (this->luminosity)++;
	pthread_mutex_unlock (&this->mutex);
}

//int
//sun_get_luminosity (sun_t* this)
//{
// int luminosity;
//pthread_mutex_lock (&this->mutex);
// luminosity = this->luminosity;
//pthread_mutex_unlock (&this->mutex);
// return luminosity;
//}

int dateToInt (sun_t* this, int param, int riseSet){
	char* editHour = NULL;
	if (riseSet == 0){
		editHour = strdup(this->sunrise);
	}else{
		editHour = strdup(this->sunset);
	}
	int hour = atoi(strtok(editHour, ":"));
	int mins = atoi(strtok(NULL, ":"));
	int secs = atoi(strtok(NULL, ":"));
	if (param == 0){
		return hour;
	}else if (param == 1){
		return mins;
	}else{
		return secs;
	}
	return 80;
}

void sendHour (sun_t* this){
	int writeFD = wiringPiI2CSetup(0x20);
	wiringPiI2CWrite(writeFD, 0xFF);
	wiringPiI2CWrite(writeFD, dateToInt(this, 0, 0));
	wiringPiI2CWrite(writeFD, dateToInt(this, 1, 0));
	wiringPiI2CWrite(writeFD, dateToInt(this, 2, 0));
	wiringPiI2CWrite(writeFD, dateToInt(this, 0, 1));
	wiringPiI2CWrite(writeFD, dateToInt(this, 1, 1));
	wiringPiI2CWrite(writeFD, dateToInt(this, 2, 1));

}

void
sun_destroy (sun_t* this)
{
	if (this->sunrise) free ((char*)this->sunrise);
	if (this->sunset) free ((char*)this->sunset);
	free (this);
}
