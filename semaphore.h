#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <native/mutex.h>
#include "observer.h"

#define MAXSEMAFORES 4

typedef enum {
	ON, OFF
} state_t;

typedef struct
{
	observable_t observable;        

	int id;
	state_t state;
	RT_MUTEX mutex;
	
}semaphore_t;
    
extern semaphore_t* semaphores[MAXSEMAFORES];

semaphore_t* semaphore_new(int id, state_t state);
void semaphore_init(semaphore_t* this, int id, state_t state);
void semaphore_setup(void);
void semaphore_switch_on(int i);
void semaphore_switch_off(int i);
#endif
        
