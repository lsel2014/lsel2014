/*
 * train.c
 *
 *  Created on: 01/04/2014
 *      Author: user
 */

#include "train.h"
#include "dcc.h"

static train_t* trains[MAXTRAINS];
static int ntrains = 0;
static train_t* current_train;

void trains
int train_cmd(char* arg) {
	if (0 == strcmp(arg, "list")) {
		printf("ID\tNAME\tPOWER\tDIRECTION\t");
		int i;
		for (i = 0; i < ntrains; ++i) {
			printf("%d\t%s\t%d\t%s\r\n", trains[i]->ID, trains[i]->name,
					trains[i]->power,
					(trains[i]->direction) == FORWARD ? "FORWARD" : "REVERSE");
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
		return 1;
	}
	if (0 == strncmp(arg, "speed ", strlen("speed "))) {
		int target_speed;
		target_speed = atoi(arg + strlen("speed "));
		train_set_target_power(current_train, target_speed);
		return 0;
	}
	return 1;
}

train_t* train_new(char* name, char ID, char n_wagon, char length,
		dcc_sender_t* dcc, telemetry_t telemetry) {
	train_t this = (train_t*) malloc(sizeof(train_t));
	train_init(this, name, ID, n_wagon, length, telemetry);
	if (ntrains < MAXTRAINS) {
		trains[ntrains++] = this;
	}
	return this;
}

void train_init(train_t* this, char* name, char ID, char n_wagon, char length,
		dcc_sender_t* dcc, telemetry_t telemetry) {
	this->name = name;
	this->ID = ID;
	this->power = 0;
	this->target_power = 0;
	this->direction = FORWARD;
	this->n_wagon = n_wagon;
	this->length = length;
	this->telemetry = telemetry;
	rt_mutex_create(&this->mutex, NULL);
}

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
	train_set_direcion(this, (power < 0) ? REVERSE : FORWARD);
	dcc_add_speed_packet(this->dcc, this->ID, this->power);
}

void train_set_target_power(train_t* this, int power) {
	this->target_power = power;
	/*
	 * In the future anti-collision system will take care of this,
	 * for now just blindly obey the user.
	 */
	train_set_power(this, power);
}

void train_set_direction(train_t* this, char direction) {
	this->direction = direction;
}

void train_set_n_wagon(train_t* this, char n_wagon) {
	this->n_wagon = n_wagon;
}

void train_set_length(train_t* this, char length) {
	this->length = length;
}

void train_set_telemetry(train_t* this, telemetry_t telemetry) {
	this->telemetry = telemetry;
}

char* train_get_name(train_t* this) {
	return this->name;
}

char train_get_ID(train_t* this) {
	return this->ID;
}

char train_get_power(train_t* this) {
	return this->power;
}

char train_get_direction(train_t* this) {
	return this->direction;
}

char train_get_n_wagon(train_t* this) {
	return this->n_wagon;
}

char train_get_length(train_t* this) {
	return this->length;
}

telemetry_t train_get_telemetry(train_t* this) {
	return this->telemetry;
}
