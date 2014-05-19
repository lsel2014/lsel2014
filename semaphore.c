#include "semaphore.h"
#include <stdlib.h>
#include <rtdk.h>

semaphore_t* semaphores[MAXSEMAFORES];
int nsemaphores;

semaphore_t* 
semaphore_new(int id)
{
    semaphore_t* this = (semaphore_t*) malloc(sizeof(semaphore_t));
	semaphore_init(this, id);
	
    if (nsemaphores < MAXSEMAFORES) {
		semaphores[nsemaphores++] = this;
	}

	return this;
}
void 
semaphore_init(semaphore_t* this, int id)
{
	this->id = id;
	observable_init(&this->observable);

	rt_mutex_create(&this->mutex, NULL);
}
void 
semaphore_setup(void)
{
    int i;
    for( i=0; 0<MAXSEMAFORES; i++){
         semaphore_new(i);
         }
}
void 
semaphore_switch_on(void)
{
    rt_printf(" switching on!!!");
}
void 
semaphore_switch_off(void)
{
    rt_printf(" switching on!!!");                          
}
