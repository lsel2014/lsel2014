#include "actuator.h"

void
actuator_init (actuator_t* this, int id, actuator_notify_func_t notify)
{
    observer_init ((observer_t*) this, actuator_notify);
    this->id = id;
    this->notify = notify;
}

void
actuator_notify (observer_t* this, observable_t* observable)
{
    this->notify (this, observable);
}
