#ifndef TRAIN_H
#define TRAIN_H

#include <native/mutex.h>
#define MAXTRAINS 8
typedef struct telemetry_t {
	char sector;
	char speed;
}telemetry_t;


typedef struct train_t {
char* name;
char ID;
char targetpower;
char power;
char direction;
char n_wagon;
char length;
telemetry_t telemetry;
RT_MUTEX mutex;
} train_t;


typedef struct {
	train_t* trains[MAXTRAINS];
	int ntrains =0;
	int current_train=0;
}model_trains_t;

train_t* train_new (char* name, char ID, char n_wagon, char length, telemetry_t telemetry);
void train_init (train_t* this, char* name, char n_wagon, char length, telemetry_t telemetry);

void train_destroy (train_t* this);
model_trains_t* trains_setup(void);

void train_set_name(train_t* this, char* name);
void train_set_ID(train_t* this, char ID);
void train_set_target_power(train_t* this, char target_power);
void train_set_power(train_t* this, char power);
void train_set_direction(train_t* this, char direction);
void train_set_n_wagon(train_t* this, char n_wagon);
void train_set_length(train_t* this, char length);
void train_set_telemetry(train_t* this, telemetry_t telemetry);
char* train_get_name(train_t* this);
char train_get_ID(train_t* this);
char train_get_power(train_t* this);
char train_get_direction(train_t* this);
char train_get_n_wagon(train_t* this);
char train_get_length(train_t* this);
void train_register(train_t* this);
telemetry_t train_get_telemetry(train_t* this);
