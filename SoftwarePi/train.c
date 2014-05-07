/**
 * @file    train.c
 * @date    2014-04-01
 * @brief   Train entity definition
 *
 * Implementation of train_t functions and declarations
 */

#include <rtdk.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#include "train.h"
#include "dcc.h"
#include "time_operations.h"

train_t* trains[MAXTRAINS];
train_t* current_train;
int ntrains = 0;

static void train_update_est(void* arg);

// Object creation/destruction -----

/**
 * @brief train_t creation
 *
 * Creates a train_t object and stores it in the
 * global trains array if not full.
 *
 * @ingroup train_t_object
 *
 * @param name      Train name
 * @param ID        Train ID
 * @param n_wagon   Number of wagons attached to the train
 * @param length    Length of the train
 * @param dcc       DCC sender reference
 *
 * @returns this    Malloc'd and initialized train_t object
 */
train_t* train_new(char* name, char ID, char n_wagon, char length,
		dcc_sender_t* dcc)
{
	train_t* this = (train_t*) malloc(sizeof(train_t));
	telemetry_t* telemetry = (telemetry_t*) malloc(sizeof(telemetry_t));
	
	train_init(this, name, ID, n_wagon, length, dcc, telemetry);

    /*
     * Add new train to global trains array
     * if array is not full
     */
	if (ntrains < MAXTRAINS) {
		trains[ntrains++] = this;
	}

	return this;
}

/**
 * @brief train_t initialization
 *
 * @ingroup train_t_object
 * 
 * @param this      Malloc'd train_t object
 * @param name      Train name
 * @param ID        Train ID
 * @param n_wagon   Number of wagons attached to the train
 * @param length    Length of the train
 * @param dcc       DCC sender reference
 * @param telemetry Telemetry data
 */
void train_init(train_t* this, char* name, char ID, char n_wagon, char length,
		dcc_sender_t* dcc, telemetry_t* telemetry)
{
	observable_init(&this->observable);
	
	this->name = name;
	this->ID = ID;
	this->target_power = 0;
	this->power = 0;
	this->direction = FORWARD;
	this->n_wagon = n_wagon;
	this->length = length;
	this->security_override = 0;
	this->telemetry = telemetry;

	this->dcc = dcc;
	rt_mutex_create(&this->mutex, NULL);

    /*
     * Initial power is 0
     */
	train_set_power(this, 0);
}

/**
 * @brief Model trains and DCC instantiation
 *
 * Sets up the DCC object (in charge of sending commands to
 * the railway amplifier) and every train present in the
 * model, attaching them to the DCC.
 *
 * It also registers all of the train commands in 
 * the intepreter.
 *
 * At the end, sets the current_train to the first one stored
 * in the global trains array.
 *
 * @ingroup train_t_object
 */
void trains_setup(void)
{
    /*
     * This will be integrated with the interpreter
     */
	dcc_sender_t* dccobject = dcc_new(13, 63000);

	train_new("Diesel", 0b0000100, '0', 20, dccobject);
	train_new("Renfe", 0b0000011, '0', 25, dccobject);
	
	/*for (i = 0; i < ntrains; i++) {
	 for (j = 0; j < nsensors; j++) {
	 observable_register_observer((observable_t*) sensors[j],
	 (observer_t*) trains[i]);
	 }
	 }*/
	
	current_train = trains[0];
	task_add("Estimation updater", 200000000, train_update_est, NULL);
	interp_addcmd("train", train_cmd, "Set train parameters");
	interp_addcmd("s", train_emergency_cmd, "Emergency stop all trains");
}

/**
 * @brief train_t destruction
 *
 * @ingroup train_t_object
 *
 * @param this train_t to be free'd
 */
void train_destroy(train_t* this) {
	if (this->telemetry)
		free(this->telemetry);
	free(this);
}


// Actions of train_t -----

/**
 * @brief Train related interpreter commands
 *
 * Train related commands. Allows the user to manage
 * and control the trains via the cmd interpreter.
 *
 * @ingroup train_t_actions
 *
 * @param arg Second command argument to be executed
 * @return 0
 */
int train_cmd(char* arg)
{
	/*
	 * list
	 *
	 * Lists every train in the model
	 */
	if (0 == strcmp(arg, "list"))
	{
		printf(
				"ID\tNAME\tPOWER\tTARGET\tDIRECTION\tSECTOR\tSECURITY\tACTIVE\n");
		int i;
		for (i = 0; i < ntrains; ++i) {
			printf("%d\t%s\t%d\t%d\t%s\t\t%d\t%d\t\t%s\r\n", trains[i]->ID,
					trains[i]->name, trains[i]->power, trains[i]->target_power,
					(trains[i]->direction) == FORWARD ? "FORWARD" : "REVERSE",
					trains[i]->telemetry->sector, trains[i]->security_override,
					(trains[i]->ID == current_train->ID) ? "<" : " ");
		}
		return 0;
	}

    /*
     * select
     *
     * Selects current train by setting
     * the variable current_train to
     * the desired option
     */
	if (0 == strncmp(arg, "select ", strlen("select ")))
	{
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

    /*
     * speed
     *
     * Sets current train to the
     * desired speed
     */
	if (0 == strncmp(arg, "speed ", strlen("speed ")))
	{
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

    /*
     * sector
     *
     * Sets current train to the
     * desired sector
     */
	if (0 == strncmp(arg, "sector ", strlen("sector ")))
	{
		int sector;
		sector = atoi(arg + strlen("sector "));
		train_set_current_sector(current_train, sector);
		return 0;
	}

    /*
     * estop
     *
     * Sends an emergency stop message to
     * the current train
     */
	if (0 == strncmp(arg, "estop", strlen("estop")))
	{
		train_emergency_stop(current_train);
		printf("Train %d %s stopped\n", current_train->ID, current_train->name);
		return 0;
	}

    /*
     * function
     *
     * Performs a specific function
     * of the current train
     */
	if (0 == strncmp(arg, "function ", strlen("function ")))
	{
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

	/*
	 * check_est
	 *
	 * @todo Code refactoring
	 */
	if (0 == strncmp(arg, "check_est", strlen("check_est")))
	{
		struct timeval initial, current, diff;
		train_get_timestamp(current_train, &initial);
		float elapsed_time, final_time;
		char time_out = 0;
		
		float initial_estimation = train_get_time_estimation(current_train);
		/*
		 * Hardcoded 20%
		 */
		float time_out_time = 1.2 * initial_estimation;

		//printf("tout %f\n", time_out_time);
		while (initial_estimation == train_get_time_estimation(current_train)
				&& !time_out) {
			gettimeofday(&current, NULL);
			timeval_sub(&diff, &current, &initial);
			elapsed_time = (float) diff.tv_sec + ((float) diff.tv_usec / 1.0E6);
			//printf("%2.7f",current_time);
			if (elapsed_time > time_out_time) {
				time_out = 1;
				//printf("TIMEOUT");
			}
		}
		//printf("ctime %f\n", elapsed_time);

		if (time_out || elapsed_time < 0.8 * initial_estimation)
		{
			printf("Estimation was off by more than 20%%\n");
			return 1;
		}
		printf("Estimation within 20%%, OK\n");
		return 0;
	}

    /*
     * wait_sector
     *
     * Wait for the current train to
     * reach a specific sector
     */
	if (0 == strncmp(arg, "wait_sector ", strlen("wait_sector ")))
	{
		int sector = atoi(arg + strlen("wait_sector"));
		if (sector < 0 || sector > 3)
		{
			printf("Sector must be between 0 and 3\n");
			return 1;
		}

		char time_out = 0;
		struct timeval t1;
		gettimeofday(&t1, NULL);
		int max_time = t1.tv_sec + 30;
		while (train_get_sector(current_train) != sector && !time_out)
		{
			gettimeofday(&t1, NULL);
			//printf("%d\n",(int)t1.tv_sec);
			if (t1.tv_sec >= max_time)
				time_out = 1;
		}

		if (time_out)
		{
			printf("Train didn't reach sector in 30 seconds\n");
			return 1;
		}

		return 0;
	}

    /*
     * help
     *
     * Help command
     */
	if (0 == strncmp(arg, "help", strlen("help")))
	{
		printf( "Available commands:\n"
				"  select <n>\t\tSelects train with ID <n>\n"
				"  speed <n>\t\tSets current train speed to <n>\n"
				"  estop\t\tEmergency stop\n"
				"  function <n> <s>\t\t<s>=1 enables DCC function <n>\n\t\t\t<s>=0 disables it\n");
		return 0;
	}

	/*
	 * Default:
	 *
	 * If no defined command is passed,
	 * print an error message
	 */
	printf(
			"Incorrect command. Use train help to see a list of available commands\n");
	return 1;
}

/**
 * @brief Emergency stop interpreter command
 *
 * Sends an emergency stop command to every train.
 * This will cut off the power to the motors and enable
 * the electric brake if available. Unrealistic deceleration.
 *
 * @ingroup train_t_actions
 *
 * @param arg Second command parameter, dummy in this case
 * @return 0
 */
int train_emergency_cmd(char* arg)
{
	int i;
	
	for (i = 0; i < ntrains; i++)
	{
		dcc_add_data_packet(trains[i]->dcc, trains[i]->ID, ESTOP_CMD);
	}

	for (i = 0; i < ntrains; i++)
	{
		train_set_target_power(trains[i], 0);
	}

	printf("All trains stopped\n");
	
	return 0;
}

/**
 * @brief train_t emergency stop
 *
 * Sends an emergency stop command to
 * the train passed as a parameter.
 *
 * @ingroup train_t_actions
 *
 * @param this Train to be stopped
 */
void train_emergency_stop(train_t* this)
{
	rt_mutex_acquire(&this->mutex, TM_INFINITE);
	this->power = 0;
	rt_mutex_release(&this->mutex);

    int i;
	for (i = 0; i < 3; i++)
	{
		dcc_add_data_packet(this->dcc, this->ID, ESTOP_CMD);
	}

}


// Getters -----

/**
 * @brief train_t name getter
 *
 * @ingroup train_t_getters
 *
 * @param this train_t object
 * @return this->name Train name
 */
char* train_get_name(train_t* this)
{
	return this->name;
}

/**
 * @brief train_t ID getter
 *
 * @ingroup train_t_getters
 *
 * @param this train_t object
 * @return this->ID Train ID
 */
char train_get_ID(train_t* this)
{
	return this->ID;
}

/**
 * @brief train_t target power getter
 *
 * @ingroup train_t_getters
 *
 * @param this train_t object
 * @return this->target_power Train target power
 */
int train_get_target_power(train_t* this)
{
	return this->target_power;
}

/**
 * @brief train_t power getter
 *
 * @ingroup train_t_getters
 *
 * @param this train_t object
 * @return power Train power
 */
int train_get_power(train_t* this)
{
	rt_mutex_acquire(&this->mutex, TM_INFINITE);
	int power = this->power;
	rt_mutex_release(&this->mutex);
	return power;
}

/**
 * @brief train_t direction getter
 *
 * @ingroup train_t_getters
 *
 * @param this train_t object
 * @return this->direction Train direction
 */
train_direction_t train_get_direction(train_t* this)
{
	return this->direction;
}

/**
 * @brief train_t number of wagons getter
 *
 * @ingroup train_t_getters
 *
 * @param this train_t object
 * @return this->n_wagon Number of wagons attached to the train
 */
char train_get_n_wagon(train_t* this)
{
	return this->n_wagon;
}

/**
 * @brief train_t length getter
 *
 * @ingroup train_t_getters
 *
 * @param this train_t object
 * @return this->length Train length
 */
char train_get_length(train_t* this)
{
	return this->length;
}

/**
 * @brief train_t timestamp getter
 *
 * @ingroup train_t_getters
 *
 * @param this train_t object
 * @param *tv Timestamp struct - timestamp will be returned here
 */
void train_get_timestamp(train_t* this, struct timeval *tv)
{
	rt_mutex_acquire(&this->mutex, TM_INFINITE);
	tv->tv_sec = this->telemetry->timestamp.tv_sec;
	tv->tv_usec = this->telemetry->timestamp.tv_usec;
	rt_mutex_release(&this->mutex);
	//copy_timeval(tv, &(this->telemetry-> timestamp));
	//return this->telemetry-> timestamp;
}

/**
 * @brief train_t security flag getter
 *
 * @ingroup train_t_getters
 *
 * @param this train_t object
 * @return this->security_override Train security flag
 */
char train_get_security(train_t* this)
{
	return this->security_override;
}

/**
 * @brief train_t time estimation getter
 *
 * @ingroup train_t_getters
 *
 * @param this train_t object
 * @return est Train time estimation
 */
float train_get_time_estimation(train_t* this)
{
	rt_mutex_acquire(&this->mutex, TM_INFINITE);
	float est = this->telemetry->time_est;
	rt_mutex_release(&this->mutex);
	return est;
}

/**
 * @brief train_t current time estimation getter
 *
 * @ingroup train_t_getters
 *
 * @param this train_t object
 * @return est Train time estimation
 */
float train_get_current_time_estimation(train_t* this)
{
	rt_mutex_acquire(&this->mutex, TM_INFINITE);
	float est = this->telemetry->current_time_est;
	rt_mutex_release(&this->mutex);
	return est;
}

/**
 * @brief train_t sector getter
 *
 * @ingroup train_t_getters
 *
 * @param this train_t object
 * @return this->telemetry->sector Train sector
 */
char train_get_sector(train_t* this)
{
	return this->telemetry->sector;
}

/**
 * @brief train_t speed getter
 *
 * @ingroup train_t_getters
 *
 * @param this train_t object
 * @return speed Train speed
 */
float train_get_speed(train_t* this)
{
	rt_mutex_acquire(&this->mutex, TM_INFINITE);
	float speed = this->telemetry->speed;
	rt_mutex_release(&this->mutex);
	return speed;
}

/**
 * @brief train_t telemetry getter
 *
 * @ingroup train_t_getters
 *
 * @param this train_t object
 * @return this->telemetry Train telemetry
 */
telemetry_t* train_get_telemetry(train_t* this)
{
	return this->telemetry;
}


// Setters -----

/**
 * @brief train_t name setter
 *
 * @ingroup train_t_setters
 *
 * @param this train_t object
 * @param name Train name
 */
void train_set_name(train_t* this, char* name)
{
	this->name = name;
}

/**
 * @brief train_t ID setter
 *
 * @ingroup train_t_setters
 *
 * @param this train_t object
 * @param ID Train ID
 */
void train_set_ID(train_t* this, char ID)
{
	this->ID = ID;
}

/**
 * @brief train_t target power setter
 *
 * @ingroup train_t_setters
 *
 * @param this train_t object
 * @param power Train target power
 */
void train_set_target_power(train_t* this, int power)
{
	rt_mutex_acquire(&this->mutex, TM_INFINITE);
	this->target_power = power;

	if (this->security_override == 0)
	{
		train_set_power(this, power);
	}
	rt_mutex_release(&this->mutex);
}

/**
 * @brief train_t power setter
 *
 * @ingroup train_t_setters
 *
 * @param this train_t object
 * @param power Train power
 */
void train_set_power(train_t* this, int power)
{
	int i;

	rt_mutex_acquire(&this->mutex, TM_INFINITE);
	
	this->power = power;
	if (power < 0) {
		train_set_direction(this, REVERSE);
	} else {
		train_set_direction(this, FORWARD);
	}

	for (i = 0; i < 3; i++)
	{
		dcc_add_speed_packet(this->dcc, this->ID, this->power);
	}

	rt_mutex_release(&this->mutex);
}

/**
 * @brief train_t direction setter
 *
 * @ingroup train_t_setters
 *
 * @param this train_t object
 * @param direction Train direction
 */
void train_set_direction(train_t* this, train_direction_t direction)
{
	this->direction = direction;
	observable_notify_observers(&this->observable);
}

/**
 * @brief train_t number of wagons setter
 *
 * @ingroup train_t_setters
 *
 * @param this train_t object
 * @param n_wagon Number of wagons attached to the train
 */
void train_set_n_wagon(train_t* this, char n_wagon)
{
	this->n_wagon = n_wagon;
}

/**
 * @brief train_t length setter
 *
 * @ingroup train_t_setters
 *
 * @param this train_t object
 * @param length Train length
 */
void train_set_length(train_t* this, char length)
{
	this->length = length;
}

/**
 * @brief train_t timestamp setter
 *
 * @ingroup train_t_setters
 *
 * @param this train_t object
 * @param *tv Timestamp to be set
 */
void train_set_timestamp(train_t* this, struct timeval *tv)
{
	rt_mutex_acquire(&this->mutex, TM_INFINITE);
	this->telemetry->timestamp.tv_sec = tv->tv_sec;
	this->telemetry->timestamp.tv_usec = tv->tv_usec;
	//copy_timeval( &(this->telemetry-> timestamp) ,tv);
	rt_mutex_release(&this->mutex);
}

/**
 * @brief train_t security flag setter
 *
 * @ingroup train_t_setters
 *
 * @param this train_t object
 * @param newSecurity Train security flag
 */
void train_set_security(train_t* this, char newSecurity)
{
	rt_mutex_acquire(&this->mutex, TM_INFINITE);

	this->security_override = newSecurity;
	if (newSecurity == 0) {
		train_set_power(this, this->target_power);
	}

	rt_mutex_release(&this->mutex);
}

/**
 * @brief train_t time estimation setter
 *
 * @ingroup train_t_setters
 *
 * @param this train_t object
 * @param estimation Train time estimation
 */
void train_set_time_estimation(train_t* this, float estimation)
{
	rt_mutex_acquire(&this->mutex, TM_INFINITE);
	this->telemetry->time_est = estimation;
	this->telemetry->current_time_est =estimation;
	rt_mutex_release(&this->mutex);
	//train_set_current_time_estimation(this,estimation);
	observable_notify_observers(&this->observable);
}

/**
 * @brief train_t time estimation setter
 *
 * @ingroup train_t_setters
 *
 * @param this train_t object
 * @param estimation Train time estimation
 */
void train_set_current_time_estimation(train_t* this, float estimation)
{
	rt_mutex_acquire(&this->mutex, TM_INFINITE);
	this->telemetry->current_time_est = estimation;
	rt_mutex_release(&this->mutex);
	observable_notify_observers(&this->observable);
}
/**
 * @brief train_t sector setter
 *
 * @ingroup train_t_setters
 *
 * @param this train_t object
 * @param sector Train sector
 */
void train_set_current_sector(train_t* this, char sector)
{
	rt_mutex_acquire(&this->mutex, TM_INFINITE);
	this->telemetry->sector = sector;
	//rt_printf ("Starting notify\n");
	rt_mutex_release(&this->mutex);
	observable_notify_observers(&this->observable);
}

/**
 * @brief train_t speed setter
 *
 * @ingroup train_t_setters
 *
 * @param this train_t object
 * @param speed Train speed
 */
void train_set_current_speed(train_t* this, float speed)
{
	rt_mutex_acquire(&this->mutex, TM_INFINITE);
	this->telemetry->speed = speed;
	rt_mutex_release(&this->mutex);
	observable_notify_observers(&this->observable);
}
static
void train_update_est(void* arg){
	int i;
	float est;
	rt_task_set_periodic(NULL, TM_NOW, 200000000);
	while (1) {
		rt_task_wait_period(NULL);
		for(i=0;i<ntrains;i++){
			est=train_get_current_time_estimation(trains[i]);
			est = (est-0.2)>0?est-0.2:0.0;
			train_set_current_time_estimation(trains[i],est);
		}
	}
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
