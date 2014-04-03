/*
 * sensorIR.c
 *
 *  Created on: 02/04/2014
 *      Author: Javier
 */

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <wiringPi.h>

#include "sensor.h"

sensor_t*
sensorIR_new (int GPIOline, int prio, int ceil)
{
	sensor_t* this = (sensor_t*) malloc (sizeof (sensor_t));
	sensorIR_init (this, GPIOline, prio, ceil);
	return this;
}

void
sensorIR_init (sensor_t* this, int GPIOline, int prio, int ceil)
{

	this->GPIOline = GPIOline;
	this->GPIOflag = LOW;
	pthread_mutex_init (&this->mutex_sensor, NULL);
	//Tareas xenomai.
}

void
sensorIR_destroy (sensor_t* this)
{
	if (this->GPIOline) free ((int*)this->GPIOline);
	if (this->GPIOflag) free ((int*)this->GPIOflag);
	pthread_mutex_destroy(&this->mutex_sensor);
	free (this);
}


int
sensorIR_get_GPIOline (sensor_t* this)
{
	//pthread_mutex_lock(&this->mutex_sensor);
	return this->GPIOline;
	//pthread_mutex_unlock(&this->mutex_sensor);
}

void
sensorIR_set_GPIOline (sensor_t* this, int newGPIOline)
{
	//pthread_mutex_lock(&this->mutex_sensor);
	this->GPIOline = newGPIOline;
	//pthread_mutex_unlock(&this->mutex_sensor);
}

int
sensorIR_get_GPIOflag (sensor_t* this)
{
	//pthread_mutex_lock(&this->mutex_sensor);
	return this->GPIOflag;
	//pthread_mutex_unlock(&this->mutex_sensor);
}

void
sensorIR_set_GPIOflag (sensor_t* this, int newGPIOflag)
{
	//pthread_mutex_lock(&this->mutex_sensor);
	this->GPIOflag = newGPIOflag;
	//pthread_mutex_unlock(&this->mutex_sensor);
}

void
sensorIR_rise_GPIOflag (sensor_t* this)
{
	//pthread_mutex_lock(&this->mutex_sensor);
	this->GPIOflag = HIGH;
	//pthread_mutex_unlock(&this->mutex_sensor);
}

void
sensorIR_clear_GPIOflag (sensor_t* this)
{
	//pthread_mutex_lock(&this->mutex_sensor);
	this->GPIOflag = LOW;
	//pthread_mutex_unlock(&this->mutex_sensor);
}

void
sensorIR_readSensor(sensor_t* this)
{
	//pthread_mutex_lock(&this->mutex_sensor);
	this->GPIOflag = digitalRead(this->GPIOline);
	//pthread_mutex_unlock(&this->mutex_sensor);
}



