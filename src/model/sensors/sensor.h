#ifndef SENSOR_H
#define SENSOR_H

#include "../../observer.h"

typedef struct sensor_t
{
    //Inheritance
    observable_t observable;

    //Attributes
    int id;

} sensor_t;


/*
 * Virtual class, no constructor
 */
void sensor_init (sensor_t* this, int id);

int sensor_get_id (sensor_t* this);
void sensor_set_id (sensor_t* this, int id);

#endif
