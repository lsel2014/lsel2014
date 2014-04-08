#include "actuator.h"

void
actuator_init (actuator_t* this, int id)
{
    observer_init ((observer_t*) this, notify);
    this->id = id;
}
