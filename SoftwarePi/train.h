#ifndef TRAIN_H
#define TRAIN_H
#define MAXTRAINS 8
#include <pthread.h>
#include <native/mutex.h>
#include "dcc.h"
#include "Interpreter/interp.h"

typedef struct telemetry_t {
	char sector;
	char speed;
} telemetry_t;

typedef enum {
	FORWARD, REVERSE
} train_direction_t;

typedef struct train_t {
	char* name;
	char ID;
	int target_power;
	int power;
	train_direction_t direction;
	char n_wagon;
	char length;
	RT_MUTEX mutex;
	dcc_sender_t* dcc;
	telemetry_t* telemetry;
} train_t;

void trains_setup(void);
train_t* train_new(char* name, char ID, char n_wagon, char length,
		dcc_sender_t* dcc);
void train_init(train_t* this, char* name, char ID, char n_wagon, char length,
		dcc_sender_t* dcc, telemetry_t* telemetry);
void train_destroy(train_t* this);
int train_cmd(char* arg);
void train_set_name(train_t* this, char* name);
void train_set_ID(train_t* this, char ID);
void train_set_target_power(train_t* this, int power);
void train_set_power(train_t* this, int power);
void train_set_direction(train_t* this, train_direction_t direction);
void train_set_n_wagon(train_t* this, char n_wagon);
void train_set_length(train_t* this, char length);
void train_set_telemetry(train_t* this, telemetry_t* telemetry);
char* train_get_name(train_t* this);
char train_get_ID(train_t* this);
int train_get_power(train_t* this);
train_direction_t train_get_direction(train_t* this);
char train_get_n_wagon(train_t* this);
char train_get_length(train_t* this);
telemetry_t* train_get_telemetry(train_t* this);

#endif
