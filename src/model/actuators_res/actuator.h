#ifndef ACTUATOR_H
#define ACTUATOR_H

#include "../observer.h"

typedef void (*actuator_notify_func_t) (observer_t*, observable_t*);

typedef struct actuator_t
{
    //Inheritance
    observer_t observer;

    //Attributes
    int id;

    //Functions
    actuator_notify_func_t notify;

} actuator_t


/*
 * Virtual class, no constructor
 */
void actuator_init (actuator_t* this, int id, actuator_notify_func_t notify);
void actuator_notify (observer_t* this, observable_t* observable);

#endif
