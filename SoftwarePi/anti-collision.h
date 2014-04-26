/*
 * anti-collision.h
 *
 *  Created on: 24/04/2014
 *      Author: Javier
 */

#ifndef ANTI-COLLISION_H
#define ANTI-COLLISION_H

#include "train.h"
#include "railway.h"
#include "Model/observer.h"

typedef struct anti-collision_t {

	observer_t* observer;
	char security_flag;
	
	//railway_t* railway;

	RT_MUTEX mutex;

} anti-collision_t;

//Object creation/destruction
anti-collision_t* anti-collision_new(void);
void anti-collision_init(anti-collision_t* this);
void anti-collision_destroy (anti-collision_t* this);

//Observer function
void anti-collision_notify (observer_t* this, observable_t* observable);



#endif