#ifndef IR_IDENTIFICATION_H
#define IR_IDENTIFICATION_H

#include "sensor.h"
#include "../actuators/train.h"
#include <native/mutex.h>
#include <wiringPi.h>

#define MAX_BEACONS 10
#define IR_DEADLINE 20000000
#define IR_PERIOD   100000000

/*
 * IR identification
 */
typedef struct ir_identification_t
{
    //Inheritance
    sensor_t sensor;

    //Attributes
    int gpio [MAX_TRAINS];
    int num_trains;
    RT_MUTEX mutex;

}

/*
 * Object creation / destruction
 */
ir_identification_t* ir_identification_new (int id, int num_trains);
void ir_identification_init (id_identification_t* this, int id, int num_trains);


/*
 * Sensor task
 */
void ir_identification_polling_task (ir_identification_t* this);
