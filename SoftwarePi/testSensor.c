/*
 * testSensor.c
 *
 *  Created on: 02/04/2014
 *      Author: Javier
 */

#include <pthread.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <wiringPi.h>

#include "sensorIR.h"

int main() {

	int cuenta;

	if (wiringPiSetup () == -1)
			exit (1) ;

	pinMode (2, INPUT);
	pinMode (3, INPUT);
	pinMode (4, OUTPUT);
	pinMode (5, OUTPUT);
	digitalWrite(4,0);
	digitalWrite(5,0);

	sensorIR_t* s1 = sensorIR_new(1, 1, 1);

	while(1) {

	  	printf("Waiting\n");
	  	delay (100);

	  	if (cuenta++ == 50) {
	  		digitalWrite(4,1);
	  	}
	  	if (cuenta == 100) {
	  		  		digitalWrite(5,1);
	  	}

	  }


	return 0;
}
