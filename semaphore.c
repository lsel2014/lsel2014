#include "semaphore.h"
#include <stdlib.h>
//#include <rtdk.h>
#include "lsquaredc.h"
#include "daemon.h"
// Random values, this will change to match the firmware of the barrier
#define I2C_SEMAPHORE_ADDRESS_0 0x22
#define I2C_SEMAPHORE_ADDRESS_1 0x23
#define I2C_SEMAPHORE_ADDRESS_2 0x24
#define I2C_SEMAPHORE_ADDRESS_3 0x2
#define I2C_SEMAPHORE_ON 0x00
#define I2C_SEMAPHORE_OFF 0x01

semaphore_t* semaphores[MAXSEMAFORES];
int nsemaphores;
static uint16_t i2c_addresses[]={I2C_SEMAPHORE_ADDRESS_0,I2C_SEMAPHORE_ADDRESS_1,I2C_SEMAPHORE_ADDRESS_2,I2C_SEMAPHORE_ADDRESS_3};

semaphore_t* 
semaphore_new(int id, int state, uint16_t i2c_address)
{
    semaphore_t* this = (semaphore_t*) malloc(sizeof(semaphore_t));
	semaphore_init(this, id , state, i2c_address);
	
    if (nsemaphores < MAXSEMAFORES) {
		semaphores[nsemaphores++] = this;
	}

	return this;
}
void 
semaphore_init(semaphore_t* this, int id, int state, uint16_t i2c_address)
{
	observable_init(&this->observable);
	this->id = id;
	this->state = state;
	this-> i2c_address=i2c_address;
	rt_mutex_create(&this->mutex, NULL);
}
void 
semaphore_setup(void)
{
    int i;
    for( i=0; i<MAXSEMAFORES; i++){
         semaphore_new(i,0,i2c_addresses[i]);
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
semaphore_switch (semaphore_t* this, int state)
{
    uint16_t semaphore_comand[2]={(this->i2c_address<<1),0x00};
    if( state == 0 ){
	semaphore_comand[1]=I2C_SEMAPHORE_OFF;
	}else{
	semaphore_comand[1]=I2C_SEMAPHORE_ON;
    } 
    rt_mutex_acquire(&this->mutex, TM_INFINITE);
    this->state = state;
    rt_mutex_release(&this->mutex);
        
    //rt_mutex_acquire(&(i2chandler[1]->mutex), TM_INFINITE);
    //i2c_send_sequence(i2chandler[1]->i2chandler, semaphore_comand, 2, 0);
    //rt_mutex_release(&i2chandler[1]->mutex);

}
