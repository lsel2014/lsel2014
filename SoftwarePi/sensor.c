#include "sensor.h"

void sensor_init(sensor_t* this, sensor_process_data_func_t process_data,
		int id) {
	observable_init((observable_t*) this);
	this->id = id;
	this->process_data = process_data;
}

int sensor_get_id(sensor_t* this) {
	return this->id;
}

void sensor_set_id(sensor_t* this, int newid) {
	this->id = newid;
}

void sensor_process_data(sensor_t* this) {
	this->process_data(this);
}

/*
  Local variables:
    mode: c
    c-file-style: stroustrup
  End:
*/
