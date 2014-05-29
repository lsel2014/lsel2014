/*
 * sensorIR.c
 *
 *  Created on: 02/04/2014
 *      Author: Javier
 */


#include <rtdk.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
//#include <wiringPi.h>

#include "lsquaredc.h"
#include "daemon.h"
#include "sensorIR.h"
// Random values, this will change to match the firmware of the barrier
#define I2C_IR_ADDRESS_0 0x22
#define I2C_IR_ADDRESS_1 0x23
#define I2C_IR_ADDRESS_2 0x24
#define I2C_IR_ADDRESS_3 0x25
#define I2C_RENFE_BYTE 0x26
#define I2C_DIESEL_BYTE 0x27

sensorIR_t* sensors[MAXSENSORS];
int nsensors = 0;
static uint16_t i2c_addresses[]={I2C_IR_ADDRESS_0,I2C_IR_ADDRESS_1,I2C_IR_ADDRESS_2,I2C_IR_ADDRESS_3};

void 
IRsensors_poll(void* arg) 
{
	//sensorIR_t** sensors = (sensorIR_t**) arg;
	rt_task_set_periodic(NULL, TM_NOW, IR_PERIOD);
	while (1) {
		int i = 0;
		rt_task_wait_period(NULL);
		for (i = 0; i < nsensors; i++) {
			sensorIR_trainPassing(sensors[i]);
		}
	}
}

int 
sensors_cmd(char*arg) 
{
	int i;
	for (i = 0; i < nsensors; i++) {
		printf("Sensor %d", sensors[i]->id);
	}
	return 0;
}

void
IRsensors_setup(void) 
{
	int i;
	for (i = 0; i < 4; i++) {
		sensorIR_new(i,i2c_addresses[i]);
	}
	task_add("IR polling", IR_DEADLINE, IRsensors_poll, sensors);
	interp_addcmd("sensors", sensors_cmd, "Lists IR sensors");
}

sensorIR_t*
sensorIR_new(int id, uint16_t i2c_address) {
	sensorIR_t* this = (sensorIR_t*) malloc(sizeof(sensorIR_t));
	event_t* event = (event_t*) malloc(sizeof(event_t));
	sensorIR_init(this, id, event, i2c_address);
	if (nsensors < MAXSENSORS) {
		sensors[nsensors++] = this;
	}
	return this;
}

void 
sensorIR_init(sensorIR_t* this, int id, event_t* event, uint16_t i2c_address) 
{
	int i;
	observable_init((observable_t *) this);
	this->id = id;
	/*for (i = 0; i < 2; i++) {
		// Set the associated train line for that sensor
		this->GPIOlines[i] = (id * 2) + i;
		// Set the line as input
		//pinMode(this->GPIOlines[i], INPUT);
		
	}*/
//	this->last_reading = -1;
    this->i2c_address = i2c_address;
	this->event = event;
	rt_mutex_create(&this->mutex, NULL);

}

void 
sensorIR_destroy(sensorIR_t* this) 
{
	free(this);
}

//int sensorIR_readLine(sensorIR_t* this, int trainLine)
void 
sensorIR_readLine(sensorIR_t* this, uint8_t* buff) 
{
	//int r;
    uint16_t read_IR_comand[]={(this->i2c_address<<1)|1 ,I2C_READ};
	// Adquire mutex
	rt_mutex_acquire(&(i2chandler[0]->mutex), TM_INFINITE);
	// Read sensor
	//r = digitalRead(this->GPIOlines[trainLine]);
    i2c_send_sequence(i2chandler[0]->i2chandler, read_IR_comand, 2, buff);
	// Release mutex
	rt_mutex_release(&i2chandler[0]->mutex);
	//return r;
}
/*
void 
sensorIR_trainPassing(sensorIR_t* this) 
{
	int i, r;
	r = -1;

	// Adquire mutex
	rt_mutex_acquire(&(this->mutex), TM_INFINITE);
	this->event->flag = 0;
	for (i = 0; i < 2; i++) {
		// Read sensor line and check its state
		if (digitalRead(this->GPIOlines[i]) == HIGH) {
			// 4 if diesel is passing, 3 if renfe is passing, or 2 if no one                                
			r = i + 3;
		}
	}
	if (r > 2) {

		this->event->flag = 1;
		this->event->passingTrain = r;
	}
	// Release mutex
	rt_mutex_release(&this->mutex);

	if (this->event->flag == 1) {
		observable_notify_observers((observable_t*) this);
		rt_mutex_acquire(&(this->mutex), TM_INFINITE);
		this->event->flag = 0;
		rt_mutex_release(&this->mutex);
	}
}
*/
void 
sensorIR_trainPassing(sensorIR_t* this)
{
     uint8_t* buff = (uint8_t*) malloc(sizeof(uint8_t));
     
     rt_mutex_acquire(&(this->mutex), TM_INFINITE);
     
     this->event->flag = 0; 
     sensorIR_readLine(this, buff); 
     // 4 if diesel is passing, 3 if renfe is passing   
     if( buff[0] == I2C_RENFE_BYTE ){
         this->event->passingTrain = 3; 
         this->event->flag = 1;
     }else if(buff[0] == I2C_DIESEL_BYTE){
         this->event->passingTrain = 3; 
         this->event->flag = 1;   
     }             
     rt_mutex_release(&this->mutex);
     
     if (this->event->flag == 1) {
		observable_notify_observers((observable_t*) this);
		rt_mutex_acquire(&(this->mutex), TM_INFINITE);
		this->event->flag = 0;
		rt_mutex_release(&this->mutex);
	}                             
}

event_t*
sensorIR_get_event(sensorIR_t* this) 
{
	return this->event;
}
