#include "observer.h"
#include <rtdk.h>
void
observer_init (observer_t* this, observer_notify_func_t notify)
{
  this->notify = notify;
}

void
observer_notify (observer_t* this, observable_t* observable)
{
  this->notify (this, observable);
}


void
observable_init (observable_t* this)
{
  this->nobservers = 0;
}

void
observable_register_observer (observable_t* this, observer_t* obs)
{
	rt_printf ("Observer registered\n");
  this->observer[this->nobservers++] = obs;
}

void
observable_notify_observers (observable_t* this)
{
  int i;
  rt_printf("Going to notify\n");
  for (i = 0; i < this->nobservers; ++i)
    rt_printf("Notifying observer no %i\n", i);
    observer_notify (this->observer[i], this);
}

