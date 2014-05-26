#include "semaphore.h"
#include <stdlib.h>
#include <rtdk.h>

semaphore_t* semaphores[MAXSEMAFORES];
int nsemaphores;

semaphore_t* 
semaphore_new(int id, state_t state)
{
    semaphore_t* this = (semaphore_t*) malloc(sizeof(semaphore_t));
	semaphore_init(this, id , state);
	
    if (nsemaphores < MAXSEMAFORES) {
		semaphores[nsemaphores++] = this;
	}

	return this;
}
void 
semaphore_init(semaphore_t* this, int id, state_t state)
{
	observable_init(&this->observable);
	this->id = id;
	this->state = state;
	rt_mutex_create(&this->mutex, NULL);
}
void 
semaphore_setup(void)
{
    int i;
    for( i=0; i<MAXSEMAFORES; i++){
         semaphore_new(i,OFF);
         }
}
void 
semaphore_switch_on(int i)
{
    semaphore_t* local = semaphores[i];
    local->state = ON;
    rt_printf(" semafore %d switched to %s \n", i , semaphores[i]->state);
}
void 
semaphore_switch_off(int i)
{
    semaphore_t* local = semaphores[i];
    local->state = OFF;
    rt_printf(" semafore %d switched to %s \n", i , semaphores[i]->state);                        
}
