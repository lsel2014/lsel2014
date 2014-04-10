#include "sensor.h"

void
sensor_init (sensor_t* this, int id)
{
    observable_init ((observable_t*) this);
    this->id = id;
}


int
sensor_get_id (sensor_t* this)
{
    return this->id;
}

void
sensor_set_id (sensor_t* this, int id)
{
    this->id = id;
}
