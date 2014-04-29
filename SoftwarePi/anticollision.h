/*
 * anticollision.h
 *
 *  Created on: 24/04/2014
 *      Author: Javier
 */

#ifndef ANTICOLLISION_H
#define ANTICOLLISION_H

#include "train.h"
#include "railway.h"
#include "Model/observer.h"
#include "Interpreter/interp.h"

typedef struct anticollision_t {

	observer_t* observer;
	char security_flag;

	//railway_t* railway;

	RT_MUTEX mutex;

} anticollision_t;

//Object creation/destruction
void anticollision_setup(void);
anticollision_t* anticollision_new(void);
void anticollision_init(anticollision_t* this);
void anticollision_destroy (anticollision_t* this);
int anticollision_cmd(char* arg);

//Observer function
void anticollision_notify (observer_t* this, observable_t* observable);





#endif
