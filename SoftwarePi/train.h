/**
 * @file    train.h
 * @date    2014-04-01
 * @brief   Train entity declaration
 *
 * Declaration of train_t object and functions
 */

/**
 * Train entity
 *
 * @defgroup train
 * @{
 */

#ifndef TRAIN_H
#define TRAIN_H

#include <pthread.h>
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>

#include "interp.h"
#include "observer.h"
#include "sensorIR.h"

#define ESTOP_CMD 0b01000001
#define MAXTRAINS 8

/**
 * Train globals
 *
 * @defgroup train_t_globals train_t globals
 * @{
 */




/**
 * Train data structures
 *
 * @defgroup train_t_structs train_t data structures
 * @{
 */

/**
 * Train runtime physical parameters. It stores
 * real speed, time, and sector of a train.
 *
 * @struct telemetry_t
 */
typedef struct
{
	char sector;
	float speed;
	struct timeval timestamp;
	float time_est;
	float current_time_est;
} telemetry_t;

/**
 * Definition of two posible directions of movement
 * for a train.
 *
 * @enum train_direction_t
 */
typedef enum
{
	FORWARD, REVERSE
} train_direction_t;




/**
 * Train model. It includes basic parameters such as name and ID,
 * number of wagons attached and total length of the model, as well
 * as runtime values like power, speed, current sector and a security
 * flag.
 *
 * @struct train_t
 */
typedef struct
{
	observable_t observable;        /**< Inheritance */
	
	char* name;
	char ID;
	int target_power;
	int power;
	train_direction_t direction;
	char n_wagon;
	char length;
	pthread_mutex_t mutex;
	telemetry_t* telemetry;
	char security_override;
} train_t;
/**@}*/

/**
 * Global array of trains in the model
 */
extern train_t* trains[MAXTRAINS];
/**
 * Number of trains in the model
 */
extern int ntrains;
/**@}*/

/**
 * Object creation/destruction functions of train_t
 *
 * @defgroup train_t_object train_t object functions
 * @{
 */
train_t*    train_new(char* name, char ID, char n_wagon, char length);
void    train_init(train_t* this, char* name, char ID, char n_wagon, char length,
		    telemetry_t* telemetry);
void    trains_setup(void);
void    train_destroy(train_t* this);
/**@}*/


/**
 * Actions of train_t
 * 
 * @defgroup train_t_actions train_t actions
 * @{
 */
int     train_cmd(char* arg);
int     train_emergency_cmd(char* arg);

void    train_emergency_stop(train_t* this);
void    train_wait_sector(train_t* this, char sector);
void    timeval_sub(struct timeval *res, struct timeval *a, struct timeval *b);
/**@}*/


/**
 * Getters of train_t
 *
 * @defgroup train_t_getters train_t getters
 * @{
 */
char*   train_get_name(train_t* this);
char    train_get_ID(train_t* this);
int     train_get_target_power(train_t* this);
int     train_get_power(train_t* this);
train_direction_t   train_get_direction(train_t* this);
char    train_get_n_wagon(train_t* this);
char    train_get_length(train_t* this);
void    train_get_timestamp(train_t* this, struct timeval *tv);
char    train_get_security(train_t* this);
float   train_get_time_estimation(train_t* this);
float   train_get_current_time_estimation(train_t* this);
char    train_get_sector(train_t* this);
float   train_get_speed(train_t* this);
telemetry_t*    train_get_telemetry(train_t* this);
/**@}*/


/**
 * Setters of train_t
 * 
 * @defgroup train_t_setters train_t setters
 * @{
 */
void    train_set_name(train_t* this, char* name);
void    train_set_ID(train_t* this, char ID);
void    train_set_target_power(train_t* this, int power);
void    train_set_power(train_t* this, int power);
void    train_set_direction(train_t* this, train_direction_t direction);
void    train_set_n_wagon(train_t* this, char n_wagon);
void    train_set_length(train_t* this, char length);
void    train_set_timestamp(train_t* this, struct timeval *tv);
void    train_set_security(train_t* this, char newSecurity);
void    train_set_time_estimation(train_t* this, float estimation);
void    train_set_current_time_estimation(train_t* this, float estimation);
void    train_set_current_sector(train_t* this, char sector);
void    train_set_current_speed(train_t* this, float speed);
/**@}*/


/**@}*/
#endif

/*
  Local variables:
    mode: c
    c-file-style: stroustrup
  End:
*/
