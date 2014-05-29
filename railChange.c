/*
 LSEL 2014

 @authors _____, J. Martin
 @date April 2014

 Represents a electrical rail changer.
 
 */

#include "railChange.h"
#include <wiringPi.h> 
#include "lsquaredc.h"
#include "daemon.h"
// Random values, this will change to match the firmware of the barrier
#define I2C_RAIL_CHANGE_ADDRESS 0x22
#define I2C_RAIL_CHANGE_LEFT 0x00
#define I2C_RAIL_CHANGE_RIGHT 0x01

static railChange_t* changer;

void 
railChange_setup(void) 
{
	changer = railChange_new( LEFT,I2C_RAIL_CHANGE_ADDRESS);
	interp_addcmd("changer", railChange_cmd, "Set rail state\n");
}

int 
railChange_cmd(char* arg) 
{
	if (0 == strcmp(arg, "list")) {
		printf("Rail changer\t%s\r\n",
				(changer->direction) == LEFT ?
						"LEFT (1) (OUTER)" : "RIGHT (2) (INNER)");

		return 0;
	}
	if (0 == strncmp(arg, "set ", strlen("set "))) {
		int state;
		state = atoi(arg + strlen("set "));
		railChange_set_direction(changer, state == 1 ? LEFT : RIGHT);
		return 1;
	}
	return 1;
}

void 
railChange_init(railChange_t* this, direction_t direction, uint16_t i2c_address) 
{
	this->direction = direction;
        this->i2c_address= i2c_address;
	rt_mutex_create(&this->mutex, NULL);
	railChange_set_direction(this, direction);

}

railChange_t*
railChange_new(direction_t direction, uint16_t i2c_address) {
	railChange_t* this = (railChange_t*) malloc(sizeof(railChange_t));
	railChange_init(this, direction,i2c_address);

	return this;

}

direction_t 
railCange_get_direction(railChange_t* this) 
{
	return this->direction;
}

void 
railChange_set_direction(railChange_t* this, direction_t direction) 
{
	uint16_t railChange_comand[]={(this->i2c_address<<1),0x00};
	if( direction == LEFT ){
	railChange_comand[1]=I2C_RAIL_CHANGE_LEFT;
	}else{
	railChange_comand[1]=I2C_RAIL_CHANGE_RIGHT;
        } 
             
        rt_mutex_acquire(&(this->mutex), TM_INFINITE);

	this->direction = direction;
	//digitalWrite(this->GPIOline, direction == LEFT ? 1 : 0);
	
	rt_mutex_release(&(this->mutex));
	
	rt_mutex_acquire(&(i2chandler[1]->mutex), TM_INFINITE);
	//i2c_send_sequence(i2chandler[1]->i2chandler, railChange_comand, 2, 0);
	rt_mutex_release(&i2chandler[1]->mutex);
}
