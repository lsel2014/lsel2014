#include <stdlib.h>
#include <native/mutex.h>
#include <stdbool.h>
#include <wiringPi.h>
#include <softPwm.h>

#include "tasks.h"
#include "crossingGate.h"
#include "interp.h"
#include "lsquaredc.h"
#include "daemon.h"
// Random values, this will change to match the firmware of the barrier
#define I2C_BARRIER_ADDRESS 0x57
#define I2C_BARRIER_UP 0x01
#define I2C_BARRIER_DOWN 0x00
#define I2C_BARRIER_STOP 0x02

crossingGate_t* crossingGates[MAXCROSINGGATES];
int n_crossingGates;
void 
crossingGate_setup(void) 
{
       // int sensec[] = {2,3};
        n_crossingGates=0;
	crossingGates[0] = crossingGate_new(0,2,I2C_BARRIER_ADDRESS);

	interp_addcmd("barrier", crossingGate_cmd, "Set barrier state\n");

}

int 
crossingGate_cmd(char* arg) 
{
	if (0 == strcmp(arg, "list")) {
		printf("Gate\t%s\r\n", (crossingGates[0]->position) == UP ? "UP" : "DOWN");

		return 0;
	}
	if (0 == strncmp(arg, "set ", strlen("set "))) {
		int state;
		state = atoi(arg + strlen("set "));
		crossingGate_set_position(crossingGates[0], state == 1 ? UP : DOWN);
		return 1;
	}
	return 1;
}

crossingGate_t*
crossingGate_new(int id, /*int sensiblesectors[2],*/ int sensiblesector, uint16_t i2c_address) 
{
	crossingGate_t* this = (crossingGate_t*) malloc(sizeof(crossingGate_t));
	crossingGate_init(this, id, DOWN,/* sensiblesectors,*/ sensiblesector,i2c_address);
        if (n_crossingGates < MAXCROSINGGATES) {
		crossingGates[n_crossingGates++] = this;
	}
	return this;
}

void 
crossingGate_init(crossingGate_t* this, int id, position_t position,
                                   /*int sensiblesectors[2],*/ int sensiblesector,  uint16_t i2c_address) 
{
	observable_init(&this->observable);
	this->position = position;
	this->id = id;
	this->needsService = 0;
	this->i2c_address=i2c_address;
        //this->sensiblesectors[0] = sensiblesectors[0];
        //this->sensiblesectors[1] = sensiblesectors[1];
	this->sensiblesector= sensiblesector;
	rt_mutex_create(&this->mutex, NULL);
	task_add("Gate control", GATE_DEADLINE, crossingGate_move_task, this);
	crossingGate_set_position(this, position);

}
void 
crossingGate_destroy(crossingGate_t* this) 
{
	//This should be done properly
	free(this);
}
int
crossingGate_get_sensiblesector(crossingGate_t* this)
{
   return this->sensiblesector;                                  
}

void 
crossingGate_set_position(crossingGate_t* this, position_t position) 
{
	rt_mutex_acquire(&(this->mutex), TM_INFINITE);
	if (this->position != position) {
		this->position = position;
		this->needsService = 1;
	}
	rt_mutex_release(&this->mutex);
	//rt_printf("gate switched");
}

void
crossingGate_set_light(crossingGate_t* this, int state){

	uint16_t barrier_comand[3]={(this->i2c_address<<1),0x01, 0x00};
        if( state==0){
	  barrier_comand[2]= 0x00;
        }else{
	   barrier_comand[2]= 0x01;
	}
	rt_mutex_acquire(&(i2chandler[0]->mutex), TM_INFINITE);
	i2c_send_sequence(i2chandler[0]->i2chandler, barrier_comand, 3, 0);
	rt_mutex_release(&(i2chandler[0]->mutex));


}
void
crossingGate_move_task(void *args) 
{
	crossingGate_t* this = (crossingGate_t*) args;
	uint16_t barrier_comand[3]={(this->i2c_address<<1),0x00, 0x00};
	rt_task_set_periodic(NULL, TM_NOW, GATE_PERIOD);
	while (1) {
		rt_task_wait_period(NULL);
		if (this->needsService) {
			if (this->position == DOWN) {
				barrier_comand[2]=I2C_BARRIER_DOWN;
			} else {
				barrier_comand[2]=I2C_BARRIER_UP;
			}
			rt_mutex_acquire(&(this->mutex), TM_INFINITE);
			this->needsService = 0;
			rt_mutex_release(&this->mutex);
		}// else {
		//	barrier_comand[1]=I2C_BARRIER_STOP;	
		//}
		rt_mutex_acquire(&(i2chandler[0]->mutex), TM_INFINITE);
		i2c_send_sequence(i2chandler[0]->i2chandler, barrier_comand, 3, 0);
		rt_mutex_release(&(i2chandler[0]->mutex));
	}
}
