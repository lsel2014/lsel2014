/*
 * anticollision.c
 *
 *  Created on: 24/04/2014
 *      Author: Javier
 */

#include <stdio.h>
#include <stdlib.h>
#include <native/mutex.h>
#include <rtdk.h>

#include "anticollision.h"
#include "train.h"
#include "railway.h"
#include "interp.h"
#include "dcc.h"
#include "model.h"

static observer_t antic_observer;

char overrides_activated;
char security_enable = 0;

// Arrays where model elements are stored locally
static struct railway_data_t {
	railway_t* railway;
	int platform;
} antic_railway[MAXRAILWAY];

static int n_railway;


/*
 * Se deberia entrar aqui siempre que un tren cambie de sector o de sentido
 */
void anticollision_notify(observer_t* this) {
	
    int i,j,k;
    int to_check;
    //int new_power;
    for(j=0; j<n_railway;j++){
	    railway_t* rail = antic_railway[j].railway;
		if (security_enable == 1) {
			for (i = 0; i < NSECTORS; i++) {
			    for(k = 0; k < rail->railwaySectors[i]->nregisteredtrains ; k++){
				//if ((rail->railwaySectors[i]->nregisteredtrains) > 0) {
					train_t* train = rail->railwaySectors[i]->registeredTrains[j]; 
					to_check = 0;
	
					//Por ahora solo comprueba el sector siguiente segun el sentido. Habra que hacerlo mejor
					if (train_get_direction(train) == FORWARD) {
						to_check = i - 1;
						if (to_check == -1)
							to_check = NSECTORS - 1;
					} else if (train_get_direction(train) == REVERSE) {
						to_check = i + 1;
						if (to_check == NSECTORS)
							to_check = 0;
					} else {
						rt_printf("error");
					}
	
					if ((rail->railwaySectors[to_check]->nregisteredtrains) > 0) {
	
						if (train_get_security(train) == 0) {
							overrides_activated++;
							train_set_security(train, 1);
						
							rt_printf("Seguridad activada en el tren ID: %d\n", train->ID);
							rt_printf("Overrides: %d\n", overrides_activated);
							//// TODO: Aqui se debería hacer algo mas interesante, por ahora se para y no hace nada mas
							train_emergency_stop(train);
						//	train_set_power(train, 0);
						}
						
					} else {
						if (train_get_security(train) == 1) {
							overrides_activated--;
							train_set_security(train, 0);
							rt_printf("Seguridad desactivada en el tren ID: %d\n",
									train->ID);
							rt_printf("Overrides: %d\n", overrides_activated);
						}
					}
				}
			}
		}
/*		if (overrides_activated == 2) {

		new_power = -train_get_target_power(trains[0]);
		train_set_target_power(trains[0], new_power);
		
		overrides_activated--;
		train_set_security(trains[0], 0);

		printf("Protocolo de mismo sentido");
    		}	*/
	}
}


void anticollision_setup(void) {
	
	static struct railway_name_t {
		const char* name;
		int platform;
	} railway_names[] = { { "via0", 0 }, { NULL, 0 } };
	
	struct railway_name_t* r;
	n_railway = 0;
	
	observer_init(&antic_observer, anticollision_notify);

    for (r = railway_names; r->name; ++r) {
		observable_t* obs = model_get_observable(r->name);
		//observable_register_observer(obs, (observer_t*) anticollision);
		observable_register_observer(obs, &antic_observer);
		antic_railway[n_railway].railway = (railway_t*) obs;
		antic_railway[n_railway].platform = r->platform;
		++n_railway;
	}
	interp_addcmd("anti", anticollision_cmd,
			"Shows the anticollisions security protocol status");
}

int anticollision_cmd(char* arg) {
	if (0 == strcmp(arg, "status")) {
		if (security_enable == 0) {
			printf("Anticollision system disabled.\n");
		} else {
			printf("Anticollision system enabled.\n");
		}
		return 0;
	}

	if (0 == strcmp(arg, "cancel")) {
		int i;
		for (i = 0; i < ntrains; i++) {
			train_set_power(trains[i], 0);
			train_set_target_power(trains[i], 0);
			train_set_security(trains[i], 0);
		}
		printf("Anticollision override cancelled in all trains.\n");
		return 0;
	}

	if (0 == strncmp(arg, "enable ", strlen("enable "))) {
		int en;
		en = atoi(arg + strlen("enable "));
		if (en == 0) {
			security_enable = 0;
			printf("Anticollision system disabled\n.");
		} else {
			security_enable = 1;
			printf("Anticollision system enabled\n.");
		}
		return 0;
	}

	printf("Incorrect command.\n");
	return 1;
}
