/*
 * train.c
 *
 *  Created on: 01/04/2014
 *      Author: user
 */

#include "train.h"



train_t* train_new (char* name, char ID,char power,char direction,
					 char n_wagon, char length, telemetry_t telemetry) {
	train_t this = (train_t*) malloc (sizeof (train_t));
	train_init (this, name, ID, power, direction,
			 n_wagon,  length, telemetry);
	return this;
}

void train_init (train_t* this, char* name, char ID,char power,char direction,
					char n_wagon, char length, telemetry_t telemetry) {
	this->name = name;
	this->ID = ID;
	this->power = power;
	this->direction = direction;
	this->n_wagon = n_wagon;
	this->length = length;
	this->telemetry = telemetry;
	pthread_mutex_init (&this->mutex, NULL);
}

void train_destroy (train_t* this) {
	if (this->telemetry) free (this->telemetry);
	free (this);
}

void train_set_name(train_t* this, char* name) {
	this->name = name;
}

void train_set_ID(train_t* this, char ID) {
	this->ID = ID;
}

void train_set_power(train_t* this, char power) {
	this->power = power;
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
