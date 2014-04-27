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

/*
 * Se deberia entrar aqui siempre que un tren cambie de sector o de sentido
 */
void anticollision_notify (observer_t* this, observable_t* observable) {
	railway_t* railway = (railway_t*) observable;
	anticollision_t* thisAC = (anticollision_t*) this;
	int i;
	
	for (i = 0; i < NSECTORS; i++) {
		if ( /*TODO: el sector i contiene algun tren dentro*/ ) {
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
			
			if ( /* TODO: el sector to_check contiene algun tren dentro */ ) {
				thisAC->security_flag = 1;
				train_set_security(train, 1);
				
				//// TODO: Aqui se debería hacer algo mas interesante, por ahora se para y no hace nada mas
				train_set_power(train, 0);
			} else {
				if (train_get_security(train) == 1) {
					train_set_security(train, 0);
				}
			}
		}
	}
	
}
