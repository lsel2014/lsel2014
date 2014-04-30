#ifndef SENSOR_H
#define SENSOR_H

#include "../observer.h"

struct sensor_t;

typedef void (*sensor_process_data_func_t)(struct sensor_t*);

typedef struct sensor_t {
	//Inheritance
	observable_t observable;

	//Attributes
	int id;

	//Functions
	sensor_process_data_func_t process_data;

} sensor_t;

/*
 * Virtual class, no constructor
 */
void sensor_init(sensor_t* this, sensor_process_data_func_t process_data,
		int id);

int sensor_get_id(sensor_t* this);
void sensor_set_id(sensor_t* this, int newid);

void sensor_process_data(sensor_t* this);

#endif
