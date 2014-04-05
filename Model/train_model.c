#include "train_model.h"

/*
 * Object creation/destruction
 */
train_model_t* train_model_new (actuator_t actuator, sensor_t sensor)
{
    train_model_t this = (train_model_t*) malloc (sizeof (train_model_t));
    train_model_init (this, actuator, sensor);
    return this;
}

void train_model_init (train_model_t* this, actuator_t actuator, sensor_t sensor);
{
    this->actuator = actuator;
    this->sensor = sensor;
}

void train_model_destroy (train_model_t* this)
{
    free (this);
}

/*
 * Getters/Setters
 */
actuator_t train_model_get_actuator (train_model_t* this)
{
    return this->actuator;
}

sensor_t train_model_get_sensor (train_model_t* this)
{
    return this->sensor;
}

void train_model_set_actuator (train_model_t* this, actuator_t actuator)
{
    this->actuator = actuator;
}

void train_model_set_sensor (train_model_t* this, sensor_t sensor)
{
    this->sensor = sensor;
}


#endif
