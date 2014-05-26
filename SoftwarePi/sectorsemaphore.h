/**
 * @file    sectorsemaphore.h
 * @date    2014-05-25
 * @brief   Sector semaphore entity declaration
 *
 * Declaration of sectorsemaphore_t object and functions
 */

/**
 * Sector semaphore entity
 *
 * @defgroup sectorsemaphore
 * @{
 */

#ifndef SECTORSEMAPHORE_H
#define SECTORSEMAPHORE_H

#include <stdio.h>
#include <stdlib.h>
#include <native/mutex.h>
#include <rtdk.h>
#include <wiringPiI2C.h>

#include "Model/observer.h"
#include "Interpreter/interp.h"
#include "railway.h"


/**
 * Sector semaphore globals
 *
 * @defgroup sectorsemaphore_t_globals sectorsemaphore_t globals
 * @{
 */




/**
 * Sector semaphore data structures
 *
 * @defgroup sectorsemaphore_t_structs sectorsemaphore_t data structures
 * @{
 */


 
/**
 * Sector semaphore model. It includes the I2C address
 * and the sector where the semaphore is.
 *
 * @struct sectorsemaphore_t
 */
typedef struct
{
	observer_t observer;        /**< Inheritance */
	
	int sector;
	char data;
	
	char i2c_address;
	int i2c_fd;
	
    RT_MUTEX mutex;
} sectorsemaphore_t;
/**@}*/


/**
 * Object creation/destruction functions of sectorsemaphore_t
 *
 * @defgroup sectorsemaphore_t_object sectorsemaphore_t object functions
 * @{
 */
void				sectorsemaphore_setup(void);
sectorsemaphore_t*	sectorsemaphore_new(int sector, char i2c_address);
void				sectorsemaphore_init(sectorsemaphore_t* this, int sector, char i2c_address);
void				sectorsemaphore_destroy(sectorsemaphore_t* this);
/**@}*/


/**
 * Actions of sectorsemaphore_t
 * 
 * @defgroup sectorsemaphore_t_actions sectorsemaphore_t actions
 * @{
 */
int 	sectorsemaphore_cmd(char* arg);
void    sectorsemaphore_send_data(sectorsemaphore_t* this);
void	sectorsemaphore_notify(observer_t* this, observable_t* observable);
/**@}*/


/**
 * Getters of sectorsemaphore_t
 *
 * @defgroup sectorsemaphore_t_getters sectorsemaphore_t getters
 * @{
 */
int		sectorsemaphore_get_sector(sectorsemaphore_t* this);
char    sectorsemaphore_get_address(sectorsemaphore_t* this);
char    sectorsemaphore_get_data(sectorsemaphore_t* this);

/**@}*/


/**
 * Setters of sectorsemaphore_t
 * 
 * @defgroup sectorsemaphore_t_setters sectorsemaphore_t setters
 * @{
 */
void    sectorsemaphore_set_sector(sectorsemaphore_t* this, int setor);
void    sectorsemaphore_set_address(sectorsemaphore_t* this, char i2c_address);
void    sectorsemaphore_set_data(sectorsemaphore_t* this, char data);
/**@}*/


/**@}*/
#endif
