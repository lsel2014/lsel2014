#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <native/mutex.h>
#include "observer.h"

#define MAXSEMAFORES 4

typedef struct
{
	observable_t observable;        

	int id;
	int state;
	RT_MUTEX mutex;
	
}semaphore_t;
    
extern semaphore_t* semaphores[MAXSEMAFORES];

semaphore_t* semaphore_new (int id, int state);
void semaphore_init (semaphore_t* this, int id, int state);
void semaphore_setup (void);
int semaphore_get_state (semaphore_t* this);
void semaphore_switch (semaphore_t* this, int state);
//void semaphore_switch_on(semaphore_t* this);
//void semaphore_switch_off(semaphore_t* this);
#endif
        
