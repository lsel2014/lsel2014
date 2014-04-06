#include "sensor.h"

void
sensor_init (sensor_t* this, sensor_process_data_func_t process_data, int id)
{
    observable_init ((observable_t*) this);
    this->id = id;
    this->process_data = process_data;
}

void
sensor_process_data (sensor_t* this)
{
    this->process_data (this);
}
