/*
 * sensor.h
 *
 *  Created on: 02/04/2014
 *      Author: Javier
 */

#ifndef SENSOR_H
#define SENSOR_H

#include <wiringPi.h>

struct sensor_t;

typedef struct sensor_t {

	int GPIOline;

	int GPIOflag;

	pthread_mutex_t mutex_sensor;

} sensor_t;

// sensorIR
sensor_t* sensorIR_new (int GPIOline, int prio, int ceil);
void sensorIR_init (sensor_t* this, int GPIOline, int prio, int ceil);
void sensorIR_destroy (sensor_t* this);

int sensorIR_get_GPIOline (sensor_t* this);
void sensorIR_set_GPIOline (sensor_t* this, int newGPIOline);

int sensorIR_get_GPIOflag (sensor_t* this);
void sensorIR_set_GPIOflag (sensor_t* this, int newGPIOflag);
void sensorIR_rise_GPIOflag (sensor_t* this);
void sensorIR_clear_GPIOflag (sensor_t* this);

void sensorIR_storeSensor(sensor_t* this);
int sensorIR_readSensor(sensor_t* this);

// sensorI2C -- to substitute with proper data
/*
sensor_t* sensorI2C_new (int I2Caddress, int prio, int ceil);
void sensorI2C_init (sensor_t* this, int I2Caddress, int prio, int ceil);
void sensorI2C_destroy (sensor_t* this);

int sensorI2C_get_I2Caddress (sensor_t* this);
void sensorI2C_set_I2Caddress (sensor_t* this, int newI2Caddress);

void sensorI2C_readSensor(sensor_t* this);
*/


#endif
