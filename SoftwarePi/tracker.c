#include "Model/observer.h"
#include "Model/Sensors/sensorIR.h"
#include "model.h"
#include "tracker.h"
#include "train.h"
#include "railway.h"
#include <sys/time.h>
#include <time.h>
#include <rtdk.h>

#define LENGHTSECTOR 10
static observer_t tracker_observer;
// Arrays where model elements are stored
static struct ir_sensor_data_t {
	sensorIR_t* sensor;
	char sectorForward;
	char sectorReverse;
} tracker_ir_sensors[MAXSENSORS];

static struct train_data_t {
	train_t* train;
	int IRsimbolicId;
	train_direction_t storedDirection;
} tracker_trains[MAXTRAINS];

static struct railway_data_t {
	railway_t* railway;
	int platform;
} tracker_railway[MAXRAILWAY];

static int n_ir_sensors;
static int n_trains;
static int n_railway;
static event_t* event;

void
timeval_sub(struct timeval *res, struct timeval *a, struct timeval *b) {
	res->tv_sec = a->tv_sec - b->tv_sec;
	res->tv_usec = a->tv_usec - b->tv_usec;
	if (res->tv_usec < 0) {
		--res->tv_sec;
		res->tv_usec += 1000000;
	}
}
// Translate the ID ir sensors returns into the actual train pointer
train_t*
tracker_gen_train(int id) {
	struct train_data_t* t;
	for (t = tracker_trains; t->train; ++t) {
		if (id == t->IRsimbolicId)
			return t->train;
	}
	return NULL;
}
train_direction_t
tracker_gen_direction(int id) {
	struct train_data_t* t;
	for (t = tracker_trains; t->train; ++t) {
		if (id == t->IRsimbolicId)
			return t->storedDirection;
	}
	return FORWARD;
}
// Does all the operations to properly update the train
void tracker_updating_train(train_t* train, char sector, telemetry_t* tel) {
	struct timeval diff, now, last;
	float speed;
	//int i;
	struct train_data_t* t;
	//for (i = 0; i < n_trains; i++) {
	//	if (event->passingTrain == tracker_trains[i].IRsimbolicId)
	//		tracker_trains[i].storedDirection = train->direction;
	//
	for (t = tracker_trains; t->train; ++t) {
		if (train_get_ID(t->train) == t->IRsimbolicId){
			t->storedDirection = train_get_direction(train);
			rt_printf(" updated train %d direction \n" , train_get_ID(train));
		}
	}
	train_set_current_sector(train, sector);
	last = train_get_timestamp(train);
	gettimeofday(&now, NULL);
	train_set_timestamp(train, now);
	timeval_sub(&diff, &now, &last);
	speed = LENGHTSECTOR / diff.tv_usec;
	train_set_current_speed(train, speed);
	rt_printf(" updated train %d, speed %d sector %d \n", 
			train_get_ID(train), train_get_speed(train) , train_get_sector(train));
}
// Registers train in the railway taking into account the direction of the train
/*void tracker_register_train(train_t* train, int sector) {
	railway_t* rail;
	rail = railways[0];
	if (train->direction == FORWARD) {
		train_set_current_sector(train, sector);
		railway_erase_train(rail,train);
		railway_register_train(rail ,train, sector);
	} else if (sector == 0) {
		train_set_current_sector(train, 3);
		railway_erase_train(rail,train);
		railway_register_train(rail,train, 3);
	} else {
		train_set_current_sector(train, sector - 1);
		railway_erase_train(rail,train);
		railway_register_train(rail,train, sector - 1);

	}
	
}
*/
// Notify checks registered sensors and if some of them
// has an event and it's not a rebound updates the model.
static
void tracker_notify(observer_t* this, observable_t* foo) {
	struct ir_sensor_data_t* sd;
//	struct train_data_t* t;
	telemetry_t* tel;
	train_t* train;
	train_direction_t storedDirection;
	int j;
	railway_t* rail;
	rail = railways[0];
	for (j = 0 ; j < n_ir_sensors; j++) {
		sd = &tracker_ir_sensors[j];
		//rt_printf("checking sensor %d, j = %d \n",sd->sensor->id, j);
		event = sensorIR_get_event(sd->sensor);
		if (event->flag == 1) {
			train = tracker_gen_train(event->passingTrain);
			storedDirection = tracker_gen_direction(event->passingTrain);
			tel = train_get_telemetry(train);
			//rt_printf("train %d", train_get_ID(train));
			if(train_get_direction(train) == FORWARD)
			{
				//rt_printf(" train FORWARD \n");
				if(train_get_direction(train) == storedDirection 
					&& sd-> sectorForward != train_get_sector(train)){
					  rt_printf(" train same dir not same sector \n");
					  tracker_updating_train(train, sd-> sectorForward, tel);
					  railway_register_train(rail, train, sd-> sectorForward);
				}else if(train_get_direction(train) != storedDirection){
					  rt_printf(" train no same dir \n");
					  tracker_updating_train(train, sd-> sectorForward, tel);
					  railway_register_train(rail, train, sd-> sectorForward);
				}
			}
			if(train_get_direction(train) == REVERSE)
			{
				//rt_printf(" train REVERSE \n");
				if(train_get_direction(train) == storedDirection 
					&& sd-> sectorReverse != tel -> sector){
					  rt_printf(" train same dir not same sector \n");	
					  tracker_updating_train(train, sd-> sectorReverse, tel);
					  railway_register_train(rail, train, sd-> sectorReverse);
				}else if(train_get_direction(train) != storedDirection){
					  rt_printf(" train no same dir \n");
					  tracker_updating_train(train, sd-> sectorReverse, tel);
					  railway_register_train(rail, train, sd-> sectorReverse);
				}
			}
		}
	}
}


void tracker_init(void) {
	// Struct with the names and associated sector of the sensors
	static struct ir_name_t {
		const char* name;
		char sectorForward;
		char sectorReverse;
	} ir_names[] = { { "IRsensor0", 0 , 3}, { "IRsensor1", 1, 0 }, { "IRsensor2", 2, 1 },
			{ "IRsensor3", 3 ,2}, { NULL, 0 } };
	// Struct with the names and the ID of each train in the IR sensors
	static struct train_name_t {
		const char* name;
		int IRsimbolicId;
	} train_names[] = { { "Diesel", 4 }, { "Renfe", 3 }, { NULL, 0 } };
	static struct railway_name_t {
		const char* name;
		int platform;
	} railway_names[] = { { "via0", 0 }, { NULL, 0 } };
	struct ir_name_t* s;
	struct train_name_t* t;
	struct railway_name_t* r;
	observer_init(&tracker_observer, tracker_notify);

	n_ir_sensors = 0;
	n_trains = 0;
	n_railway = 0;
	// those "for" takes the elements in the model, cast them to 
	// his type ( since all the elements in the model are observables)
	// and store them in the appropriate struct.
	for (s = ir_names; s->name; ++s) {
		observable_t* obs = model_get_observable(s->name);
		observable_register_observer(obs, &tracker_observer);
		tracker_ir_sensors[n_ir_sensors].sensor = (sensorIR_t*) obs;
		tracker_ir_sensors[n_ir_sensors].sectorForward = s->sectorForward;
		tracker_ir_sensors[n_ir_sensors].sectorReverse = s->sectorReverse;
		++n_ir_sensors;
	}

	for (t = train_names; t->name; ++t) {
		observable_t* obs = model_get_observable(t->name);
		observable_register_observer(obs, &tracker_observer);
		tracker_trains[n_trains].train = (train_t*) obs;
		tracker_trains[n_trains].IRsimbolicId = t->IRsimbolicId;
		tracker_trains[n_trains].storedDirection = FORWARD;
		++n_trains;
	}
	for (r = railway_names; r->name; ++r) {
		observable_t* obs = model_get_observable(r->name);
		observable_register_observer(obs, &tracker_observer);
		tracker_railway[n_railway].railway = (railway_t*) obs;
		tracker_railway[n_railway].platform = r->platform;
		++n_railway;
	}
}
