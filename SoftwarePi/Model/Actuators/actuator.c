#include "actuator.h"

void
actuator_init (actuator_t* this, int id, actuator_notify_func_t notify)
{
    observer_init ((observer_t*) this, notify);
    this->id = id;
}
