#include "observer.h"

/*
 * Observer
 */
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


/*
 * Observable
 */
void
observable_init (observable_t* this)
{
    this->num_observers = 0;
}

void
observable_register_observer (observable_t* this, observer_t* observer)
{
    this->observer_list[this->num_observers++] = observer;
}

void
observable_notify_observers (observable_t* this)
{
    int i;
    for (i = 0; i < (this->num_observers); i++)
    {
        /*
         * Notify every observer calling their
         * notify function and passing the entire
         * observable (i.e. the sensor object) as
         * a parameter
         */
        observer_notify (this->observer_list[i], this);
    }
}
