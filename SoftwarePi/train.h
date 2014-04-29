#ifndef TRAIN_H
#define TRAIN_H

#include <pthread.h>
#include <native/mutex.h>
#include <sys/time.h>
#include <time.h>
#include "dcc.h"
#include "Interpreter/interp.h"
#include "Model/observer.h"
#include "Model/Sensors/sensorIR.h"

#define ESTOP_CMD 0b01000001
#define MAXTRAINS 8

typedef struct{
	char sector;
	float speed;
	struct timeval timestamp;
} telemetry_t;

typedef enum {
	FORWARD, REVERSE
} train_direction_t;

typedef struct {
	//observer_t observer;
	observable_t observable;
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
	char security_override;
} train_t;

extern train_t* trains[MAXTRAINS];
extern int ntrains;

void trains_setup(void);
train_t* train_new(char* name, char ID, char n_wagon, char length,
		dcc_sender_t* dcc);
void train_init(train_t* this, char* name, char ID, char n_wagon, char length,
		dcc_sender_t* dcc, telemetry_t* telemetry);
void train_destroy(train_t* this);
//void train_notify(observer_t* this, observable_t* observed);
int train_cmd(char* arg);
int train_emergency_cmd(char*arg);
void train_set_name(train_t* this, char* name);
void train_set_ID(train_t* this, char ID);
void train_set_target_power(train_t* this, int power);
void train_set_power(train_t* this, int power);
void train_set_direction(train_t* this, train_direction_t direction);
void train_set_n_wagon(train_t* this, char n_wagon);
void train_set_length(train_t* this, char length);
void train_set_current_sector(train_t* this, char sector);
void train_set_current_speed(train_t* this, float speed);
void train_set_timestamp(train_t* this,struct timeval timestamp);
char* train_get_name(train_t* this);
char train_get_ID(train_t* this);
int train_get_power(train_t* this);
train_direction_t train_get_direction(train_t* this);
char train_get_n_wagon(train_t* this);
char train_get_length(train_t* this);
telemetry_t* train_get_telemetry(train_t* this);
char train_get_sector (train_t* this);
struct timeval train_get_timestamp (train_t* this);
float train_get_speed(train_t* this);
char train_get_security(train_t* this);
void train_set_security(train_t* this, char newSecurity);

#endif
