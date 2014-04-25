#include <stdlib.h>
#include "Model/Sensors/sensorIR.h"
#include "train.h"
#include "model.h"
#include "Interpreter/interp.h"

#define MAXOBSERVABLES 20
//#define IR_DEADLINE 20000000
//#define IR_PERIOD 100000000
/*
static struct registered_ir_sensor_t {
		sensorIR_t* sensor;
		const char* name;
} ir_sensorsmodel [MAXIRSENSORS];

static struct registered_train_t {
		train_t* train;
		const char* name;
} trainsmodel [MAXTRAINS];

static int n_ir_sensors;
static int n_trains;
*/
static struct registered_observable_t {
		observable_t* obs;
		const char* name;
} observablesmodel [MAXOBSERVABLES];
static int n_observable;
/*int 
sensors_cmd(char*arg){
	int i;
	for(i=0; i<n_ir_sensors; i++){
		printf("Sensor %s\n",ir_sensorsmodel[i].name);
	}
	return 0;
}

void IRsensors_poll(void* arg) {
	sensorIR_t** sensors = (sensorIR_t**) arg;
	rt_task_set_periodic(NULL, TM_NOW, IR_PERIOD);
	struct registered_ir_sensor_t* s;
	while (1) {
		int i ;
		s = ir_sensorsmodel;
		rt_task_wait_period(NULL);
		for (i = 0; i < n_ir_sensors; i++) {
			sensorIR_trainPassing(s->sensor);
			++s;
		}
	}
}

int
model_add_IRsensor (const char* name, sensorIR_t* sensor)
{
    if( n_ir_sensors <= MAXIRSENSORS ){
    ir_sensorsmodel[n_ir_sensors].name = name;
    ir_sensorsmodel[n_ir_sensors].sensor = sensor;
    return 1;
    }
    printf("Max. sensor registered\n");
    return 0;
}
*/
int
model_add_observable (const char* name, observable_t* obs)
{
    if( n_observable <= MAXOBSERVABLES ){
    observablesmodel[n_observable].name = name;
    observablesmodel[n_observable].obs = obs;
    return 1;
    }
    printf("Max. sensor registered\n");
    return 0;
}

void
model_init (void)
{
    static struct ir_name_t {
			const char* name;
			int ID;
	} ir_names [] = {
			{ "IRsensor[0]" , 0},
			{ "IRsensor[1]" , 1},
			{ "IRsensor[2]" , 2},
			{ "IRsensor[3]" , 3},
			{ NULL, -1 }
	};   
    n_observable=0;
    int i = 0;
    IRsensors_setup();
    struct ir_name_t* s;
	for (s = ir_names; s->name; ++s) {
		model_add_observable ( s->name,  sensors[i] );
		++n_observable;
		i++;
	}
	//task_add("IR polling", IR_DEADLINE, IRsensors_poll, ir_sensorsmodel);
	//interp_addcmd("sensors",sensors_cmd,"Lists IR sensors");
	
}

observable_t*
model_get_obs (const char* name) 
{
     struct registered_observable_t* o;
     for (o = observablesmodel; o->name; ++o) {
         if ( name == o-> name )
         return o->obs;
     }
}	
	
/*observable_t*
model_get_IRsensor (const char* name)
{
     struct registered_ir_sensor_t* s;
     for (s = ir_sensorsmodel; s->name; ++s) {
         if ( name == s-> name )
         return s->sensor;
     }
}
observable_t*
model_get_train (const char* name)
{
     struct registered_train_t* t;
     for (t = trainsmodel; t->name; ++t) {
         if ( name == t-> name )
         return t->train;
     }
}
*/
