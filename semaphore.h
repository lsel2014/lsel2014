#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <native/mutex.h>
#include "observer.h"

#define I2C_SEMAPHORE_ADDRESS_0 0x30
#define I2C_SEMAPHORE_ADDRESS_1 0x31
#define I2C_SEMAPHORE_ADDRESS_2 0x32
#define I2C_SEMAPHORE_ADDRESS_3 0x33
#define I2C_SEMAPHORE_GREEN	0x01
#define I2C_SEMAPHORE_YELLOW	0x02
#define I2C_SEMAPHORE_RED 	0x03

#define MAXSEMAFORES 4

typedef struct
{
	observable_t observable;        

	int id;
	uint16_t state;
	uint16_t i2c_address;
	RT_MUTEX mutex;
	
}semaphore_t;
    
extern semaphore_t* semaphores[MAXSEMAFORES];

semaphore_t* semaphore_new (int id, uint16_t state, uint16_t i2c_address);
void semaphore_init (semaphore_t* this, int id, uint16_t state,uint16_t i2c_address);
void semaphore_setup (void);
uint16_t semaphore_get_state (semaphore_t* this);
void semaphore_set_state(semaphore_t* this, uint16_t state);
void semaphore_switch (semaphore_t* this);

#endif
        
