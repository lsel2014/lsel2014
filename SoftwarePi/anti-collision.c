/*
 * anti-collision.c
 *
 *  Created on: 24/04/2014
 *      Author: Javier
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <native/mutex.h>

#include "anti-collision.h"


anti-collision_t* anti-collision_new(void) {
	anti-collision_t* this = (anti-collision_t*) malloc(sizeof(anti-collision_t));
	anti-collision_init(this);
	
	return this;
}

void anti-collision_init(anti-collision_t* this) {
	observer_init((observer_t*) this, anti-collision_notify);
	security_flag = 0;
	
	//this->railway = railway;
	
	//// TODO: registrarse como observer del railway
	
	rt_mutex_create(&this->mutex, NULL);
}

void anti-collision_destroy(anti-collision_t* this) {
	//This should be done properly
	free(this);
}

/*
 * Se deberia entrar aqui siempre que un tren cambie de sector o de sentido
 */
void anti-collision_notify (observer_t* this, observable_t* observable) {
	railway_t* railway = (railway_t*) observable;
	anti-collision_t* thisAC = (anti-collision_t*) this;
	int i;
	
	for (i = 0; i < NSECTORS; i++) {
		if ( /*TODO: el sector i contiene algun tren dentro*/ ) {
			traint_t* train = railway->railwaySectors[i]->registeredTrains[0]; //probablemente, registeredTrains[0] dara errores
			int to_check;
			
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
				
				//// TODO: Aqui se debería hacer algo mas interesante, por ahora se para y ya
				train_set_power(train, 0);
			} else {
				if (train_get_security(train) == 1) {
					train_set_security(train, 0);
					int target_power = train_get_target_power(train);
					train_set_target_power(train, target_power);
				}
			}
		}
	}
	
}
