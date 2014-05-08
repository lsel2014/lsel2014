#ifndef TASK_H
#define TASK_H

#include <time.h>
#include <sys/time.h>
#include <pthread.h>
#include "utils.h"

void task_setup (void);

pthread_t task_new (const char* name, void *(*f)(void *),
                    int period_ms, int deadline_ms, int stacksize);
void task_register_time (pthread_t tid, struct timeval *time);
struct timeval *task_get_period (pthread_t tid);
struct timeval *task_get_deadline (pthread_t tid);

void mutex_init (pthread_mutex_t* m);

void task_setup_priorities (void);
void task_delete_all (void);


#endif


/*
  Local variables:
    mode: c
    c-file-style: stroustrup
  End:
*/
