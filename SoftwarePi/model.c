#include <stdlib.h>
#include "Model/Sensors/sensorIR.h"
#include "train.h"
#include "model.h"

#define MAXIRSENSORS 4
#define MAXTRAINS 8

static struct registered_ir_sensor_t {
		sensorIR_t* sensor;
		const char* name
} ir_sensors [MAXIRSENSORS];

static struct registered_train_t {
		train_t* train;
		const char* name
} trains [MAXTRAINS];

static int n_ir_sensors;
static int n_trains;

int 
sensors_cmd(char*arg){
	int i;
	for(i=0; i<n_ir_sensors; i++){
		printf("Sensor %d\n",ir_sensors[i]->name);
	}
	return 0;
}

void IRsensors_poll(void* arg) {
	sensorIR_t** sensors = (sensorIR_t**) arg;
	rt_task_set_periodic(NULL, TM_NOW, IR_PERIOD);
	while (1) {
		int i = 0;
		rt_task_wait_period(NULL);
		for (i = 0; i < nsensors; i++) {
			sensorIR_trainPassing(ir_sensors[i]->sensor);
		}
	}
}

int
model_add_IRsensor (const char* name, sensorIR_t* sensor)
{
    if( n_ir_sensors <= MAXIRSENSORS ){
    ir_sensors[n_ir_sensors].name = name;
    ir_sensors[n_ir_sensors].sensor = sensor;
   	return 1;
    }
    printf(	"Max. sensor registered\n");
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
    n_ir_sensors=0; 
    struct ir_name_t* s;
	for (s = ir_names; s->name; ++s) {
		model_add_IRsensor( s->name,  sensorIR_new(s->ID) );
		++n_ir_sensors;
	}
	task_add("IR polling", IR_DEADLINE, IRsensors_poll, ir_sensors);
	interp_addcmd("sensors",sensors_cmd,"Lists IR sensors");
}
observable_t*
model_get (const char* name)
{
	return NULL;
}