/*
 LSEL 2014

 @authors _____, J. Martin
 @date April 2014

 Represents a electrical rail changer.
 
 */

#include "railChange.h"
#include <wiringPi.h> 

// Random values, this will change to match the firmware of the barrier
#define I2C_RAIL_CHANGE_ADRESS 0x22
#define I2C_RAIL_CHANGE_LEFT 0x00
#define I2C_RAIL_CHANGE_RIGHT 0x01

static railChange_t* changer;

void railChange_setup(void) {
	changer = railChange_new( LEFT,I2C_RAIL_CHANGE_ADRESS);
	interp_addcmd("changer", railChange_cmd, "Set rail state\n");
}

int railChange_cmd(char* arg) {
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

void railChange_init(railChange_t* this, direction_t direction, uint16_t i2c_address) {
	this->direction = direction;
	//this->GPIOline = GPIOline;
    	this->i2c_address= i2c_address;
	//pinMode(GPIOline, OUTPUT);
	rt_mutex_create(&this->mutex, NULL);
	railChange_set_direction(this, direction);

}

railChange_t*
railChange_new(direction_t direction, uint16_t i2c_address) {
	railChange_t* this = (railChange_t*) malloc(sizeof(railChange_t));
	railChange_init(this, direction,i2c_address);

	return this;

}

direction_t railCange_get_direction(railChange_t* this) {
	return this->direction;
}

void railChange_set_direction(railChange_t* this, direction_t direction) {
	uint16_t barrier_comand[]={(this->i2c_address<<1),0x00};
	if( direction == LEFT ){
	barrier_comand[1]=I2C_RAIL_CHANGE_LEFT;
	}else{
	barrier_comand[1]=I2C_RAIL_CHANGE_RIGHT;
    	} 
             
    	rt_mutex_acquire(&(this->mutex), TM_INFINITE);

	this->direction = direction;
	//digitalWrite(this->GPIOline, direction == LEFT ? 1 : 0);
	i2c_send_sequence(i2c0handle, barrier_comand, 2, 0);
	
	rt_mutex_release(&(this->mutex));
}
