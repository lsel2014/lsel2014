#include <stdlib.h>
#include "sensorIR.h"
#include "train.h"
#include "model.h"
#include "interp.h"
#include "railway.h"
#include "crossingGate.h"
#include "semaphore.h"
#include "trafficLight.h"

#define MAXOBSERVABLES 30

static struct registered_observable_t {
	observable_t* obs;
	const char* name;
} observablesmodel[MAXOBSERVABLES];

static int n_observable;

int model_add_observable(const char* name, observable_t* obs) {
	if (n_observable <= MAXOBSERVABLES) {
		observablesmodel[n_observable].name = name;
		observablesmodel[n_observable].obs = obs;
		return 1;
	}
	printf("Max. sensor registered\n");
	return 0;
}

void model_init(void) {
     
	static struct ir_name_t {
		const char* name;
	} ir_names[] = { { "IRsensor0" }, { "IRsensor1" }, { "IRsensor2" }, {
			"IRsensor3" }, { NULL } };
			
	static struct train_name_t {
		const char* name;
	} train_names[] = { { "Diesel" }, { "Renfe" }, { NULL } };
	
	static struct railway_name_t {
		const char* name;
	} railway_names[] = { { "via0" }, { NULL } };
	
	static struct crossingGate_name_t {
		const char* name;
	} crossingGate_names[] = { { "barrera0" }, { NULL } };
	
	static struct trafficlight_name_t {
		const char* name;
	} trafficlight_names[] = { { "semaforotrafico0" }, { NULL } };
	
	static struct semaphore_name_t {
		const char* name;
	} semaphore_names[] = { { "semaforo0" }, { "semaforo1" }, { "semaforo2" },
                             { "semaforo3" }, { NULL } };
	n_observable = 0;
	int i = 0;
	struct ir_name_t* s;
	struct train_name_t* t;
	struct railway_name_t* r;
	struct crossingGate_name_t* cr;
	struct trafficlight_name_t* tl;
	struct semaphore_name_t* sph;
	
	for (s = ir_names; s->name; ++s) {
		model_add_observable(s->name, (observable_t*) sensors[i]);
		++n_observable;
		i++;
	}
	
	i = 0;
	for (t = train_names; t->name; ++t) {
		model_add_observable(t->name, (observable_t*) trains[i]);
		++n_observable;
		i++;
	}
	i = 0;
		for (r = railway_names; r->name; ++r) {
		model_add_observable(r->name, (observable_t*) railways[i]);
		++n_observable;
		i++;
	}
	i = 0;
	for (cr = crossingGate_names; cr->name; ++cr) {
		model_add_observable(cr->name, (observable_t*) crossingGates[i]);
		++n_observable;
		i++;
	}

	for (tl = trafficlight_names; tl->name; ++tl) {
		model_add_observable(tl->name, (observable_t*) trafficLights[i]);
		++n_observable;
		i++;
	}
	
	for (sph = semaphore_names; sph->name; ++sph) {
		model_add_observable(sph->name, (observable_t*) semaphores[i]);
		++n_observable;
		i++;
	}
}

observable_t*
model_get_observable(const char* name) {
	struct registered_observable_t* o;
	for (o = observablesmodel; o->name; ++o) {
		if (0 == strcmp(name, o->name))
			return o->obs;
	}
	printf("observable not found \n");
	return NULL;
}
