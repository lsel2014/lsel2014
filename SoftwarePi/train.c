/*
 * train.c
 *
 *  Created on: 01/04/2014
 *      Author: user
 */

#include "train.h"
#include "dcc.h"
#include <rtdk.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

train_t* trains[MAXTRAINS];
int ntrains = 0;
train_t* current_train;

/*This will be integrated with the interpreter*/
void trains_setup(void) {
	//int i, j;
	dcc_sender_t* dccobject = dcc_new(12, 50);

	train_new("Diesel", 0b0000100, '0', 20, dccobject);
	train_new("Renfe", 0b0000011, '0', 25, dccobject);
	/*for (i = 0; i < ntrains; i++) {
	 for (j = 0; j < nsensors; j++) {
	 observable_register_observer((observable_t*) sensors[j],
	 (observer_t*) trains[i]);
	 }
	 }*/
	current_train = trains[0];
	interp_addcmd("train", train_cmd, "Set train parameters");
	interp_addcmd("s", train_emergency_cmd, "Emergency stop all trains");
}
/**
 * train_emergency_cmd
 *
 * Sends a emergency stop command all available trains.
 * This will cut off the power to the motors and enable
 * the electric brake if available. Unrealistic deceleration.
 *
 * @param arg subcmd string, dummy in this case
 * @return always 0
 */
int train_emergency_cmd(char*arg) {
	int i;
	for (i = 0; i < ntrains; i++) {
		dcc_add_data_packet(trains[i]->dcc, trains[i]->ID, ESTOP_CMD);
	}
	for (i = 0; i < ntrains; i++) {
		train_set_target_power(trains[i], 0);

	}
	printf("All trains stopped\n");
	return 0;
}

/**
 * train_cmd
 *
 * Train related commands. Allows the user to manage
 * and control the trains via the cmd interpreter.
 *
 * @param arg subcmd string.
 * @return always 0
 */

int train_cmd(char* arg) {
	if (0 == strcmp(arg, "list")) {
		printf("ID\tNAME\tPOWER\tTARGET\tDIRECTION\tSECTOR\tSECURITY\tACTIVE\n");
		int i;
		for (i = 0; i < ntrains; ++i) {
			printf("%d\t%s\t%d\t%d\t%s\t%d\t%d\t%s\r\n", trains[i]->ID, trains[i]->name,
					trains[i]->power, trains[i]->target_power,
					(trains[i]->direction) == FORWARD ? "FORWARD" : "REVERSE",
					trains[i]->telemetry->sector,
					trains[i]->security_override,
					(trains[i]->ID == current_train->ID) ? "<" : " ");
		}
		return 0;
	}
	if (0 == strncmp(arg, "select ", strlen("select "))) {
		int train_id, i;
		train_id = atoi(arg + strlen("select "));
		for (i = 0; i < ntrains; i++) {
			if (trains[i]->ID == train_id) {
				current_train = trains[i];
				return 0;
			}
		}
		printf(
				"Train not found. Use train list to see a list of available trains\n");
		return 1;
	}
	if (0 == strncmp(arg, "speed ", strlen("speed "))) {
		int target_speed;
		target_speed = atoi(arg + strlen("speed "));
		if (abs(target_speed) > 28) {
			printf("Speed must be between -28 and 28\n");
			return 1;
		} else {
			train_set_target_power(current_train, target_speed);
			printf("Train %d %s speed set to %d\n", current_train->ID,
					current_train->name, target_speed);
		}
		return 0;
	}

	if (0 == strncmp(arg, "sector ", strlen("sector "))) {
		int sector;
		sector = atoi(arg + strlen("sector "));
		train_set_current_sector(current_train, sector);
		return 0;
	}

	if (0 == strncmp(arg, "estop", strlen("estop"))) {
		dcc_add_data_packet(current_train->dcc, current_train->ID, ESTOP_CMD);
		train_set_target_power(current_train, 0);
		printf("Train %d %s stopped\n", current_train->ID, current_train->name);
		return 0;
	}
	if (0 == strncmp(arg, "function ", strlen("function "))) {
		int function, state;
		sscanf(arg + strlen("function "), "%d %d", &function, &state);
		if (function < 13 && function >= 0) {
			dcc_add_function_packet(current_train->dcc, current_train->ID,
					function, state);
			printf("Train %d %s function %d\n", current_train->ID,
					current_train->name, function);
			return 0;
		}
		printf("Only functions F0 to F12 available\n");
		return 1;
	}
	if (0 == strncmp(arg, "help", strlen("help"))) {
		printf(
				"Available commands:\nselect <n>\t\tSelects train with ID <n>\nspeed <n>\t\tSets current train speed to <n>\nestop\t\tEmergency stop\nfunction <n> <s>\t\t<s>=1 enables DCC function <n>\n\t\t\t<s>=0 disables it\n");
		return 0;
	}

	printf(
			"Incorrect command. Use train help to see a list of available commands\n");
	return 1;
}

train_t* train_new(char* name, char ID, char n_wagon, char length,
		dcc_sender_t* dcc) {
	train_t* this = (train_t*) malloc(sizeof(train_t));
	telemetry_t* telemetry = (telemetry_t*) malloc(sizeof(telemetry_t));
	train_init(this, name, ID, n_wagon, length, dcc, telemetry);
	if (ntrains < MAXTRAINS) {
		trains[ntrains++] = this;
	}
	return this;
}

void train_init(train_t* this, char* name, char ID, char n_wagon, char length,
		dcc_sender_t* dcc, telemetry_t* telemetry) {
	//observer_init((observer_t *) this, train_notify);
	observable_init(&this->observable);
	this->name = name;
	this->ID = ID;
	this->power = 0;
	this->target_power = 0;
	this->security_override = 0;
	this->direction = FORWARD;
	this->n_wagon = n_wagon;
	this->length = length;
	this->dcc = dcc;
	this->telemetry = telemetry;
	rt_mutex_create(&this->mutex, NULL);
}
/*
 void train_notify(observer_t* this, observable_t* observed) {

 sensorIR_t* sensor = (sensorIR_t*)observed;
 train_t* thisTrain = (train_t*)this;
 //rt_printf ("Received train %d in sector %d\n", sensor->id, sensor->last_reading);
 if(sensor->last_reading == thisTrain->ID) {
 int newSector;
 switch (thisTrain->direction)
 {
 case FORWARD:
 newSector = sensor->id;
 break;
 case REVERSE:
 newSector = sensor->id -1;
 if (newSector == -1)
 newSector = 3;
 break;
 }
 if (thisTrain->telemetry->sector != newSector)
 {
 if (thisTrain->target_power != 0)
 train_set_current_sector (thisTrain, newSector);
 //rt_printf ("Train %i passed to sector %i\n", thisTrain->ID, newSector);
 }


 }
 }
 */
void train_destroy(train_t* this) {
	if (this->telemetry)
		free(this->telemetry);
	free(this);
}

void train_set_name(train_t* this, char* name) {
	this->name = name;
}

void train_set_ID(train_t* this, char ID) {
	this->ID = ID;
}

void train_set_power(train_t* this, int power) {
	this->power = power;
	if (power < 0) {
		train_set_direction(this, REVERSE);
	} else {
		train_set_direction(this, FORWARD);
	}
	dcc_add_speed_packet(this->dcc, this->ID, this->power);
}

void train_set_target_power(train_t* this, int power) {
	this->target_power = power;
	
	if (this->security_override == 0) {
		train_set_power(this, power);
	}
}

void train_set_direction(train_t* this, train_direction_t direction) {
	this->direction = direction;
}

void train_set_n_wagon(train_t* this, char n_wagon) {
	this->n_wagon = n_wagon;
}

void train_set_length(train_t* this, char length) {
	this->length = length;
}

void train_set_current_sector(train_t* this, char sector) {
	rt_mutex_acquire(&this->mutex, TM_INFINITE);
	this->telemetry->sector = sector;
	//rt_printf ("Starting notify\n");
	observable_notify_observers(&this->observable);
	rt_mutex_release(&this->mutex);
}

void train_set_current_speed(train_t* this, float speed) {
	rt_mutex_acquire(&this->mutex, TM_INFINITE);
	this->telemetry->speed = speed;
	rt_mutex_release(&this->mutex);
}

void train_set_timestamp(train_t* this, struct timeval *tv) {
	rt_mutex_acquire(&this->mutex, TM_INFINITE);
	this-> telemetry-> timestamp -> tv_sec = tv->tv_sec;
	this-> telemetry-> timestamp -> tv_usec  = tv->tv_usec;
	//copy_timeval( &(this->telemetry-> timestamp) ,tv);
	rt_mutex_release(&this->mutex);
}

char* train_get_name(train_t* this) {
	return this->name;
}

char train_get_ID(train_t* this) {
	return this->ID;
}

int train_get_power(train_t* this) {
	return this->power;
}

train_direction_t train_get_direction(train_t* this) {
	return this->direction;
}

char train_get_n_wagon(train_t* this) {
	return this->n_wagon;
}

char train_get_length(train_t* this) {
	return this->length;
}

telemetry_t* train_get_telemetry(train_t* this) {
	return this->telemetry;
}

char train_get_sector (train_t* this)
{
	return this->telemetry-> sector;
}
void train_get_timestamp (train_t* this, struct timeval *tv)
{
	tv->tv_sec =  this-> telemetry-> timestamp -> tv_sec;
	tv->tv_usec = this-> telemetry-> timestamp -> tv_usec;
	//copy_timeval(tv, &(this->telemetry-> timestamp));
	//return this->telemetry-> timestamp;
}
float train_get_speed(train_t* this)
{	
	return this->telemetry-> speed;
}

char train_get_security(train_t* this) {
	return this->security_override;
}

void train_set_security(train_t* this, char newSecurity) {
	rt_mutex_acquire(&this->mutex, TM_INFINITE);

	this->security_override = newSecurity;
	if(newSecurity == 0) {
		train_set_power(this, this->target_power);
	}

	rt_mutex_release(&this->mutex);
}
