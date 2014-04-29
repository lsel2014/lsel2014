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

static anticollision_t *anticollision;

void anticollision_setup(void) {
	anticollision = anticollision_new();
	interp_addcmd("anticollision", anticollision_cmd, "Shows the anticollisions security protocol status");

	observable_register_observer(&(railways[0]->observable), (observer_t*) anticollision);
}

anticollision_t* anticollision_new(void) {
	anticollision_t* this = (anticollision_t*) malloc(sizeof(anticollision_t));
	anticollision_init(this);

	return this;
}

void anticollision_init(anticollision_t* this) {
	observer_init((observer_t*) this, anticollision_notify);
	this->security_flag = 0;

	rt_mutex_create(&this->mutex, NULL);
}

void anticollision_destroy(anticollision_t* this) {
	//This should be done properly
	free(this);
}

int anticollision_cmd(char* arg) {
	if (anticollision->security_flag == 0) { //Probablemente mal, repasar
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
	int nalarm = 0;

	for (i = 0; i < NSECTORS; i++) {
		if ( (railway->railwaySectors[i]->nregisteredtrains) > 0 ) {
			train_t* train = railway->railwaySectors[i]->registeredTrains[0]; //probablemente, registeredTrains[0] dara errores
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
				nalarm++;
				printf("Seguridad activada en el tren ID: %c\n", train->ID);

				//// TODO: Aqui se debería hacer algo mas interesante, por ahora se para y no hace nada mas
				train_set_power(train, 0);
				if (nalarm == 2) {
					int j;
					for (j=0; j<ntrains; j++) {
						if (train_get_direction(trains[j]) == REVERSE) {
							train_set_power(trains[j], 20);
							train_set_target_power(trains[j], 20);
						}	
					}
					
				}
			} else {
				if (train_get_security(train) == 1) {
					train_set_security(train, 0);

					//// TODO: Comprobar si se deberia bajar el flag global
					printf("Seguridad desactivada en el tren ID: %c\n", train->ID);
				}
			}
			

		}
	}
	
}
