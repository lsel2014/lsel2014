#ifndef SUN_H
#define SUN_H


typedef const char* sun_date_t;
typedef struct{
	char hours;
	char minutes;
	char seconds;
}sun_time_t;


#define sun_date_cmp strcmp


typedef struct sun_t {

	char i2c_address;
	int i2c_fd;
	int current_simulated_time;
	sun_date_t date;
	sun_time_t sunrise;
	sun_time_t sunset;
	pthread_mutex_t mutex;
} sun_t;

sun_t* sun_new (sun_date_t date, char i2c_address, int prio, int ceil);
void sun_init (sun_t* this, sun_date_t date, char i2c_address, int prio, int ceil);
void sun_set_date (sun_t* this, sun_date_t date);
void sun_update_simulated_time(sun_t* this);
int sun_get_simulated_time(sun_t* this);
void sendHour (sun_t* this);
void sun_destroy (sun_t* this);


#endif
