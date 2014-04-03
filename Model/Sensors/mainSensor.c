/*
 * mainSensor.c
 *
 *  Created on: 02/04/2014
 *      Author: Javier
 */

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <wiringPi.h>

#include "sensor.h"

//just for testing
int main() {

	int pin;

	if (wiringPiSetup () == -1)
			exit (1) ;
	for (pin = 0 ; pin < 8 ; pin++)
		pinMode (pin, INPUT);

	sensor_t* s1 = sensorIR_new(3, 1, 1);

	int line;
	int flag;

	line = sensorIR_get_GPIOline(s1);
	printf("%d\n", line);
	sensorIR_set_GPIOline(s1, 8);
	line = sensorIR_get_GPIOline(s1);
	printf("%d\n", line);

	flag = sensorIR_get_GPIOflag(s1);
	printf("%d\n", flag);
	sensorIR_rise_GPIOflag(s1);
	flag = sensorIR_get_GPIOflag(s1);
	printf("%d\n", flag);
	sensorIR_set_GPIOflag(s1,LOW);
	flag = sensorIR_get_GPIOflag(s1);
	printf("%d\n", flag);
	flag = sensorIR_readSensor(s1);
	printf("%d\n", flag);





	return 0;
}
