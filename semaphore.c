#include "semaphore.h"
#include <stdlib.h>
#include <rtdk.h>

semaphore_t* semaphores[MAXSEMAFORES];
int nsemaphores;

semaphore_t* 
semaphore_new(int id, int state)
{
    semaphore_t* this = (semaphore_t*) malloc(sizeof(semaphore_t));
	semaphore_init(this, id , state);
	
    if (nsemaphores < MAXSEMAFORES) {
		semaphores[nsemaphores++] = this;
	}

	return this;
}
void 
semaphore_init(semaphore_t* this, int id, int state)
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
         semaphore_new(i,0);
         }
}
int 
semaphore_get_state(semaphore_t* this)
{
    rt_mutex_acquire(&this->mutex, TM_INFINITE);
    return this->state;
    rt_mutex_release(&this->mutex);
}
void 
semaphore_switch_on(int i)
{
    semaphore_t* local = semaphores[i];
    rt_mutex_acquire(&local->mutex, TM_INFINITE);
    local->state = 1;
    rt_mutex_release(&local->mutex);
    //rt_printf(" semafore %d switched to %d (ON)\n", i , semaphores[i]->state);
}
void 
semaphore_switch_off(int i)
{
    semaphore_t* local = semaphores[i];
    rt_mutex_acquire(&local->mutex, TM_INFINITE);
    local->state = 0;
    rt_mutex_release(&local->mutex);
    //rt_printf(" semafore %d switched to %d (OFF)\n", i , semaphores[i]->state);                        
}
