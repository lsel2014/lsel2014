#ifndef OBSERVER_H
#define OBSERVER_H

#define MAXOBSERVERS 10

/*
 * Observer
 */
typedef void (*observer_notify_func_t) (observer_t*, observable_t*);

typedef struct observer_t
{
    observer_notify_func_t notify;
}

/*
 * As observer_t is a virtual class,
 * there is no constructor (no observer_new)
 */
void observer_init (observer_t* this, observer_notify_func_t notify);
void observer_notify (observer_t* this, observable_t* observable);


/*
 * Observable
 */
typedef struct observable_t
{
    observer_t* observer_list[MAXOBSERVERS];
    int num_observers;
}

/*
 * Another virtual class, no constructor
 */
void observable_init (observable_t* this);
void observable_register_observer (observable_t* this, observer_t* observer);
void observable_notify_observers (observable_t* this);


#endif
