#ifndef OBSERVER_H
#define OBSERVER_H

#include <stdio.h>

#define MAXOBSERVERS 10

struct observable_t;
struct observer_t;

typedef void (*observer_notify_func_t)(struct observer_t*);

typedef struct observer_t {
	observer_notify_func_t notify;
} observer_t;

void observer_init (observer_t* this, observer_notify_func_t notify);
void observer_notify (observer_t* this);

typedef struct observable_t {
	observer_t* observer[MAXOBSERVERS];
	int nobservers;
	int nevents;
	void (*dump) (struct observable_t*, FILE*);
} observable_t;

void observable_init (observable_t* this);
void observable_register_observer (observable_t* this, observer_t* observer);
void observable_notify_observers (observable_t* this);
int observable_get_nobservers (observable_t* this);
int observable_get_nevents (observable_t* this);
void observable_dump (observable_t* this, FILE* f);

#endif

/*
  Local variables:
    mode: c
    c-file-style: stroustrup
  End:
*/
