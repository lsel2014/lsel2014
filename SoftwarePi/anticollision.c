/*
 * anticollision.c
 *
 *  Created on: 24/04/2014
 *      Author: Javier
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <native/mutex.h>

#include "anticollision.h"


void anticollision_setup(void) {
	anticollision_new();
	interp_addcmd("anticollision", anticollision_cmd, "Shows the anticollisions security protocol status");
}

anticollision_t* anticollision_new(void) {
	anticollision_t* this = (anticollision_t*) malloc(sizeof(anticollision_t));
	anticollision_init(this);
	
	return this;
}

void anticollision_init(anticollision_t* this) {
	observer_init((observer_t*) this, anticollision_notify);
	security_flag = 0;
	
	//this->railway = railway;
	
	//// TODO: registrarse como observer del railway
	
	rt_mutex_create(&this->mutex, NULL);
}

void anticollision_destroy(anticollision_t* this) {
	//This should be done properly
	free(this);
}

int anticollision_cmd(char* arg) {
	if (security_flag == 0) { //Probablemente mal, repasar
		printf("Anticollisions security protocol inactive\n");
	} else {
		printf("Anticollisions security protocol active\n");
	}
	return 0;
}

/*
 * Se deberia entrar aqui siempre que un tren cambie de sector o de sentido
 */
void anticollision_notify (observer_t* this, observable_t* observable) {
	railway_t* railway = (railway_t*) observable;
	anticollision_t* thisAC = (anticollision_t*) this;
	int i;
	int direction;
	int speed;
	int direction2;
	int speed2;
	
	for (i = 0; i < NSECTORS; i++) {
		if ( (railway->railwaySectors[i]->nregisteredtrains) > 0 ) {
			traint_t* train = railway->railwaySectors[i]->registeredTrains[0]; //probablemente, registeredTrains[0] dara errores
			int to_check;
			
			//Por ahora solo comprueba el sector siguiente segun el sentido. Habra que hacerlo mejor
			if (train_get_direction(train) == FORWARD){
				to_check = i+1;
				if (to_check == NSECTORS)
					to_check = 0;
			} else {
				to_check = i-1;
				if (to_check == -1)
					to_check = NSECTORS-1;
			}
			
			
			if ( (railway->railwaySectors[to_check]->nregisteredtrains) > 0 ) {
				if (thisAC->security_flag == 0) {
					thisAC->security_flag = 1;
				}
				train_set_security(train, 1);
				printf("Seguridad activada en el tren ID: %c\n", train->ID);
				
				//Guardar estado del tren para reactivarlo
				direction = train_get_direction(train);
				speed = train_get_power(train);
				//// TODO: Aqui se debería hacer algo mas interesante, por ahora se para y no hace nada mas
				train_set_power(train, 0);
				
				//Protocolo de actuación cuando ambos trenes van en distinto sentido
				if (train_get_direction(train) != train_get_direction("train2")) {
					direction2 = train_get_direction("train2");
					speed2 = train_get_power("train2");
					train_set_power("train2", 0);
				}
				
			} else {
				if (train_get_security(train) == 1) {
					train_set_security(train, 0);
					//Cuando se desactiva la seguridad, sigue en el estado que tenía
					train_set_direction(train, direction);
					train_set_power(train,speed);
					//// TODO: Comprobar si se deberia bajar el flag global
					printf("Seguridad desactivada en el tren ID: %c\n", train->ID);
				}
			}
			
			//Protocolo de actuación cuando los trenes tienen sentido opuesto
			////En este caso, hay que actuar sobre los dos trenes
		}
	}
	
}
