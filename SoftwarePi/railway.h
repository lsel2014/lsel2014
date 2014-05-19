/**
 * @file    railway.h
 * @date    --
 * @brief   Railway entity declaration
 *
 * Declaration of railway_t and sector_t objects and functions
 */

/**
 * Railway entity
 *
 * @defgroup railway
 * @{
 */

#ifndef RAILWAY_H
#define RAILWAY_H

#include <native/mutex.h>

#include "train.h" 
#include "Model/observer.h"
#include "Interpreter/interp.h"

#define NSECTORS 4
#define MAXRAILWAY 4

/**
 * Railway globals
 *
 * @defgroup railway_t_globals railway_t globals
 * @{
 */





/**
 * Railway data structures
 *
 * @defgroup railway_t_structs railway_t data structures
 * @{
 */

/**
 * Railway sector abstraction. It has an ID and contains
 * a list with all the trains passing through itself.
 *
 * @struct sector_t
 */
typedef struct
{
	int id;
	
	train_t* registeredTrains[MAXTRAINS];
	int nregisteredtrains;
} sector_t;

/**
 * Railway model. Consists of a list of sectors.
 *
 * @struct railway_t
 */
typedef struct
{
	observable_t observable;        /**< Inheritance */

	int id;
	sector_t* railwaySectors[NSECTORS];

	RT_MUTEX mutex;
} railway_t;
/**@}*/

/**
 * Global array of railways in the model
 */
extern railway_t* railways[MAXRAILWAY];
/**
 * Number of railwais in the model
 */
extern int nrailways;
/**@}*/


/**
 * Object creation/destruction functions of railway_t
 *
 * @defgroup railway_t_object railway_t object functions
 * @{
 */
railway_t* railway_new(int id);
void railway_init(railway_t* this, int id);
void railways_setup(void);
/**@}*/


/**
 * Object creation/destruction functions of sector_t
 *
 * @defgroup sector_t_object sector_t object functions
 * @{
 */
sector_t* sector_new(int id);
void sector_init(sector_t* this, int id);
/**@}*/


/**
 * Actions of railway_t
 *
 * @defgroup railway_t_actions railway_t actions
 * @{
 */
int railway_cmd(char* arg);
void railway_register_train(railway_t* this, train_t* train, int sector);
void railway_erase_train(railway_t* this, train_t* train);
/**@}*/


/**@}*/
#endif
