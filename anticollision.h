/*
 * anticollision.h
 *
 *  Created on: 24/04/2014
 *      Author: Javier
 */

#ifndef ANTICOLLISION_H
#define ANTICOLLISION_H

#include "observer.h"

//Object creation/destruction
void anticollision_setup(void);
int anticollision_cmd(char* arg);

//Observer function
void anticollision_notify(observer_t* this);

#endif
