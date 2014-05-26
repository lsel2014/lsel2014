/**
 * @file    sectorsemaphore.c
 * @date    2014-05-25
 * @brief   Sector semaphore entity definition
 *
 * Implementation of sectorsemaphore_t functions and declarations
 */

#include "sectorsemaphore.h"

/**
 * @brief sectorsemaphore_t instantiation
 *
 * Creates the semaphores, initializes them and registers
 * the semaphore commands to the interpreter
 *
 * @ingroup sectorsemaphore_t_object
 *
 * @param sector		Sector where the semaphore will be
 * @param i2c_address	I2C address of the semaphore
 */
void	sectorsemaphore_setup(void)
{
	//sectorsemaphore = sectorsemaphore_new(sector, i2c_address);
	
	int i;
	//Hard-coded for 4 semaphores. Adjust as needed.
	sectorsemaphore_t* semaphores[4];
	semaphores[0] = sectorsemaphore_new(0, 0x30);
	semaphores[1] = sectorsemaphore_new(1, 0x31);
	semaphores[2] = sectorsemaphore_new(2, 0x32);
	semaphores[3] = sectorsemaphore_new(3, 0x33);

	interp_addcmd("sec_sem", sectorsemaphore_cmd,
			"Sector semaphores related commands");

	for (i = 0; i < 4; i++) {
		observable_register_observer(&(railways[0]->observable),
			(observer_t*) semaphores[i]);
	}
}

/**
 * @brief sectorsemaphore_t creation
 *
 * Creates a sectorsemaphore_t object
 *
 * @ingroup sectorsemaphore_t_object
 *
 * @param sector		Sector where the semaphore will be
 * @param i2c_address	I2C address of the semaphore
 *
 * @returns this    Malloc'd and initialized sectorsemaphore_t object
 */
sectorsemaphore_t*	sectorsemaphore_new(int sector, char i2c_address)
{
	sectorsemaphore_t* this = (sectorsemaphore_t*) malloc(sizeof(sectorsemaphore_t));
	
	sectorsemaphore_init(this, sector, i2c_address);

	return this;
}

/**
 * @brief sectorsemaphore_t initialization
 *
 * @ingroup sectorsemaphore_t_object
 * 
 * @param this      	Malloc'd sectorsemaphore_t object
 * @param sector		Sector where the semaphore will be
 * @param i2c_address	I2C address of the semaphore
 */
void	sectorsemaphore_init(sectorsemaphore_t* this, int sector, char i2c_address)
{
	observer_init((observer_t*) this, sectorsemaphore_notify);
	
	this->sector = sector;
	this->i2c_address = i2c_address;
	this->i2c_fd = wiringPiI2CSetup(i2c_address);
	
	rt_mutex_create(&this->mutex, NULL);
}

/**
 * @brief sectorsemaphore_t destruction
 *
 * @ingroup sectorsemaphore_t_object
 *
 * @param this sectorsemaphore_t to be free'd
 */
void sectorsemaphore_destroy(sectorsemaphore_t* this) {
	free(this);
}


// Actions of sectorsemaphore_t -----

/**
 * @brief Sector semaphore related interpreter commands
 *
 * Sector semaphore related commands. Allows the user to manage
 * and control the semaphores via the cmd interpreter.
 *
 * @ingroup sectorsemaphore_t_actions
 *
 * @param arg Second command argument to be executed
 * @return 0
 */
int		sectorsemaphore_cmd(char* arg)
{
	printf(
			"No commands available yet for semaphores\n");
	return 1;
}

/**
 * @brief Method that sends data to the semaphore through the I2C bus
 *
 * @ingroup sectorsemaphore_t_actions
 *
 * @param this sectorsemaphore_t object
 * @param data Data to be sent through I2C
 */
void    sectorsemaphore_send_data(sectorsemaphore_t* this)
{
	wiringPiI2CWrite(this->i2c_fd, this->data);
}

/**
 * @brief Observer method for sectorsemaphore
 *
 * This method is called each time the observed railway notifies,
 * then checks if any semaphore should change, and actualizes it.
 *
 * @ingroup sectorsemaphore_t_actions
 *
 * @param this sectorsemaphore_t object, cast into observer
 * @param observable railway_t object, cast into observable
 */
void	sectorsemaphore_notify(observer_t* this, observable_t* observable)
{
	railway_t* rail = (railway_t*) observable;
	sectorsemaphore_t* thisSS = (sectorsemaphore_t*) this;
	char newData = sectorsemaphore_get_data(thisSS);
	
	if ((rail->railwaySectors[thisSS->sector]->nregisteredtrains) > 0) {
		train_t* train = rail->railwaySectors[thisSS->sector]->registeredTrains[0]; //probablemente, registeredTrains[0] dara errores
		
		if (train_get_security(train) == 1) {
			newData = 3;		//Blinking red
		} else {
			newData = 2;		//Yellow
		}
	} else {
		newData = 1;			//Green
	}
	
	if (newData != sectorsemaphore_get_data(thisSS)) {
		sectorsemaphore_set_data(thisSS, newData);
		sectorsemaphore_send_data(thisSS);
	}
}

// Getters -----

/**
 * @brief sectorsemaphore_t name getter
 *
 * @ingroup sectorsemaphore_t_getters
 *
 * @param this sectorsemaphore_t object
 * @return this->sector Semaphore sector
 */
int		sectorsemaphore_get_sector(sectorsemaphore_t* this)
{
	return this->sector;
}

/**
 * @brief sectorsemaphore_t i2c_address getter
 *
 * @ingroup sectorsemaphore_t_getters
 *
 * @param this sectorsemaphore_t object
 * @return this->i2c_address Semaphore I2C address
 */
char    sectorsemaphore_get_address(sectorsemaphore_t* this)
{
	return this->i2c_address;
}

/**
 * @brief sectorsemaphore_t data getter
 *
 * @ingroup sectorsemaphore_t_getters
 *
 * @param this sectorsemaphore_t object
 * @return this->data Semaphore data
 */
char    sectorsemaphore_get_data(sectorsemaphore_t* this)
{
	return this->data;
}

// Setters -----

/**
 * @brief sectorsemaphore_t name setter
 *
 * @ingroup sectorsemaphore_t_setters
 *
 * @param this sectorsemaphore_t object
 * @param sector Semaphore sector
 */
void    sectorsemaphore_set_sector(sectorsemaphore_t* this, int sector)
{
	this->sector = sector;
}

/**
 * @brief sectorsemaphore_t i2c_address setter
 *
 * @ingroup sectorsemaphore_t_setters
 *
 * @param this sectorsemaphore_t object
 * @param i2c_address Semaphore I2C address
 */
void    sectorsemaphore_set_address(sectorsemaphore_t* this, char i2c_address)
{
	this->i2c_address = i2c_address;
	this->i2c_fd = wiringPiI2CSetup(i2c_address);
}

/**
 * @brief sectorsemaphore_t data setter
 *
 * @ingroup sectorsemaphore_t_setters
 *
 * @param this sectorsemaphore_t object
 * @param data Semaphore data
 */
void    sectorsemaphore_set_data(sectorsemaphore_t* this, char data)
{
	this->data = data;
}
