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

static anticollision_t anticollision;
char security_enable = 0;

void anticollision_setup(void)
{
  int i;
  anticollision_init (&anticollision);
  interp_addcmd("anti", anticollision_cmd,
                "Shows the anticollisions security protocol status");

  observable_register_observer(&(railways[0]->observable),
                               (observer_t*) anticollision);
    
  for (i=0;i<ntrains;i++) {
    observable_register_observer (&(trains[i]->observable),
                                  (observer_t*) anticollision);
  }
}

anticollision_t* anticollision_new(void)
{
	anticollision_t* this = (anticollision_t*) malloc(sizeof(anticollision_t));
	anticollision_init(this);

	return this;
}

void anticollision_init(anticollision_t* this) {
	observer_init((observer_t*) this, anticollision_notify);
	security_enable = 1;
	this->overrides_activated = 0;

	rt_mutex_create(&this->mutex, NULL);
}

void anticollision_destroy(anticollision_t* this) {
	//This should be done properly
	free(this);
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

/*
 * Se deberia entrar aqui siempre que un tren cambie de sector o de sentido
 */
void anticollision_notify(observer_t* this, observable_t* observable) {

	railway_t* rail = (railway_t*) observable;
	//railway_t* rail;
	//rail = railways[0];
	anticollision_t* thisAC = (anticollision_t*) this;
	int i;

	if (security_enable == 1) {
		for (i = 0; i < NSECTORS; i++) {
			if ((rail->railwaySectors[i]->nregisteredtrains) > 0) {
				train_t* train = rail->railwaySectors[i]->registeredTrains[0]; //probablemente, registeredTrains[0] dara errores
				int to_check = 0;

				//rt_printf("Comprobando tren %s en sector %d\n",
				//		train_get_name(train), i);

				//Por ahora solo comprueba el sector siguiente segun el sentido. Habra que hacerlo mejor
				if (train_get_direction(train) == FORWARD) {
					to_check = i + 1;
					if (to_check == NSECTORS)
						to_check = 0;
				} else {
					to_check = i - 1;
					if (to_check == -1)
						to_check = NSECTORS - 1;
				}

				if ((rail->railwaySectors[to_check]->nregisteredtrains) > 0) {

					if (train_get_security(train) == 0) {
						thisAC->overrides_activated++;
						train_set_security(train, 1);
					
						printf("Seguridad activada en el tren ID: %d\n", train->ID);

						//// TODO: Aqui se debería hacer algo mas interesante, por ahora se para y no hace nada mas
						train_emergency_stop(train);
					//	train_set_power(train, 0);
					}
					
				} else {
					if (train_get_security(train) == 1) {
						thisAC->overrides_activated--;
						train_set_security(train, 0);

						printf("Seguridad desactivada en el tren ID: %d\n",
								train->ID);
					}
				}

			}
		}
	
	if (thisAC->overrides_activated == 2) {
		int new_power;
		new_power = -train_get_target_power(trains[0]);
		train_set_target_power(trains[0], new_power);
		
		thisAC->overrides_activated--;
		train_set_security(trains[0], 0);

		printf("Protocolo de mismo sentido");
	}
	
	
	}
}

/*
  Local variables:
    mode: c
    c-file-style: stroustrup
  End:
*/
