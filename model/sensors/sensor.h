#ifndef SENSOR_H
#define SENSOR_H

#include "../observer.h"

typedef void (*sensor_process_data_func_t) (sensor_t*);

typedef struct sensor_t
{
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
void sensor_init (sensor_t* this, int id, sensor_process_data_func_t process_data);
void sensor_process_data (sensor_t* this);


#endif
