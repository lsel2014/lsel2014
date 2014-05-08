/**
 * @file    railway.c
 * @date    --
 * @brief   Railway entity definition
 *
 * Implementation of railway_t and sector_t functions and declarations
 */

#include <stdio.h>
#include "railway.h"
#include "train.h"

railway_t* railways[MAXRAILWAY];
int nrailways;


// Object creation/destruction -----

/**
 * @brief railway_t creation
 *
 * Creates a railway_t object and stores it in the
 * global railways array if not full.
 *
 * @ingroup railway_t_object
 *
 * @param id        Railway ID
 *
 * @returns this    Malloc'd and initialized railway_t object
 */
railway_t* railway_new(int id)
{
	railway_t* this = (railway_t*) malloc(sizeof(railway_t));
	railway_init(this, id);

    /*
     * Add new railway to global railways array
     * if array is not full
     */
    if (nrailways < MAXRAILWAY) {
		railways[nrailways++] = this;
	}

	return this;
}

/**
 * @brief railway_t initialization
 *
 * @ingroup railway_t_object
 *
 * @param this      Malloc'd railway object
 * @param id        Railway ID
 */
void railway_init(railway_t* this, int id)
{
	int i, j, int_sector;
	char char_sector;
	this->id = id;
	observable_init(&this->observable);
	for (i = 0; i < NSECTORS; i++) {
		this->railwaySectors[i] = sector_new(i);
	}
	for (j = 0; j < ntrains; j++) {
		char_sector = train_get_sector(trains[j]);
		int_sector = (int) char_sector;
		railway_register_train(this, trains[j], int_sector);
	}
	mutex_init (&this->mutex);
}

/**
 * @brief Railway instantiation
 *
 * Sets up the railway object of the model.
 *
 * It also registers the railway commands in
 * the intepreter.
 *
 * @ingroup railway_t_object
 */
void railways_setup(void)
{
	railway_new(0);

	interp_addcmd("railway", railway_cmd, "Lists railway sectors");
}



/**
 * @brief sector_t creation
 *
 * Creates a sector_t object.
 *
 * @ingroup sector_t_object
 *
 * @param id        Sector ID
 *
 * @returns this    Malloc'd and initialized sector_t object
 */
sector_t* sector_new(int id)
{
	sector_t* this = (sector_t*) malloc(sizeof(sector_t));
	sector_init(this, id);
	return this;
}

/**
 * @brief sector_t initialization
 *
 * @ingroup sector_t_object
 *
 * @param this      Malloc'd sector_t object
 * @param id        Sector ID
 */
void sector_init(sector_t* this, int id)
{
	int i;
	this->id = id;

	/*
	 * Initializes registeredTrains array
	 */
	for (i = 0; i < MAXTRAINS; i++)
    {
		this->registeredTrains[i] = NULL;
    }
	this->nregisteredtrains = 0;

}


// Actions of railway_t -----

/**
 * @brief Railway related interpreter commands
 *
 * Railway related commands. Allows the user to manage
 * and control the railway via the cmd intepreter.
 *
 * @ingroup railway_t_actions
 *
 * @param arg Second command argument to be executed
 * @return 0
 */
int railway_cmd(char* arg)
{
	int i, j, k;
	for (i = 0; i < nrailways; i++) {
		for (j = 0; j < NSECTORS; j++) {
			printf("via %d : Sector %d :\n", railways[i]->id,
					railways[i]->railwaySectors[j]->id);
			for (k = 0; k < railways[i]->railwaySectors[j]->nregisteredtrains;
					k++)

				if ((railways[i]->railwaySectors[j]->nregisteredtrains) > 0) {
					printf("\t train %d\n",
							train_get_ID(
									railways[i]->railwaySectors[j]->registeredTrains[k]));
				} else {
					printf("\t sin trenes");
				}
		}
	}

	return 0;
}

/**
 * @brief Register a train in a railway
 *
 * Registers a given train in a given sector, storing it
 * into the desired railway.
 *
 * @ingroup railway_t_actions
 *
 * @param this      Railway to register a train in
 * @param train     Train to be registered
 * @param sector    Specific railway sector to register the train in
 */
void railway_register_train(railway_t* this, train_t* train, int sector)
{
	int rs;
	rs = this->railwaySectors[sector]->nregisteredtrains;

	if (rs < MAXTRAINS) {
		pthread_mutex_lock (&this->mutex);
		this->railwaySectors[sector]->registeredTrains[rs] = train;
		this->railwaySectors[sector]->nregisteredtrains++;
		observable_notify_observers((observable_t *) this);
		pthread_mutex_unlock (&this->mutex);
	}
}

/**
 * @brief Erase a train from a railway
 *
 * Removes a given train from the desired railway.
 *
 * @ingroup railway_t_actions
 *
 * @param this      Railway to remove a train from
 * @param train     Train to be removed
 */
void railway_erase_train(railway_t* this, train_t* train)
{
	int i, j, k;
	for (i = 0; i < NSECTORS; i++) {
		for (j = 0; j < this->railwaySectors[i]->nregisteredtrains; j++) {
			if (train_get_ID(train)
					== train_get_ID(
							this->railwaySectors[i]->registeredTrains[j])) {
				pthread_mutex_lock (&this->mutex);
				this->railwaySectors[i]->registeredTrains[j] = NULL;
				for (k = j; k < MAXTRAINS - 1; k++)
					this->railwaySectors[i]->registeredTrains[k] =
							this->railwaySectors[i]->registeredTrains[k + 1];
				this->railwaySectors[i]->nregisteredtrains--;
				pthread_mutex_unlock (&this->mutex);
			}
		}
	}
}

/*
  Local variables:
    mode: c
    c-file-style: stroustrup
  End:
*/
