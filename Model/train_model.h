#ifndef TRAIN_MODEL_H
#define TRAIN_MODEL_H

#include "actuators/actuator.h"
#include "sensors/sensor.h"

typedef
struct
train_model_t {
    actuator_t actuator;
    sensor_t sensor;
} train_model_t;


/*
 * Object creation/destruction
 */
train_model_t* train_model_new (actuator_t actuator, sensor_t sensor);
void train_model_init (train_model_t* this, actuator_t actuator, sensor_t sensor);
void train_model_destroy (train_model_t* this);

/*
 * Getters/Setters
 */
actuator_t train_model_get_actuator (train_model_t* this);
sensor_t train_model_get_sensor (train_model_t* this);

void train_model_set_actuator (train_model_t* this, actuator_t actuator);
void train_model_set_sensor (train_model_t* this, sensor_t sensor);


#endif
