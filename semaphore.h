#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <native/mutex.h>
#include "observer.h"

#define MAXSEMAFORES 4

typedef struct
{
	observable_t observable;        

	int id;

	RT_MUTEX mutex;
	
}semaphore_t;
    
extern semaphore_t* semaphores[MAXSEMAFORES];

semaphore_t* semaphore_new(int id);
void semaphore_init(semaphore_t* this, int id);
void semaphore_setup(void);
void semaphore_switch_on(void);
void semaphore_switch_off(void);
#endif
        
