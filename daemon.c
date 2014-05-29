#include <stdio.h>
#include <signal.h>
#include <unistd.h>

// Xenomai native skin libraries
#include <sys/mman.h>
#include <native/task.h>
#include <native/timer.h>
// WiringPi
#include <wiringPi.h>
//i2c
#include "lsquaredc.h"
// RT compilant print library
#include <rtdk.h>

// Tasks
#include "dcc.h"
//#include "poll.h"
//#include "sunTasks.h"

// Interpreter
#include "interp.h"
#include "daemon.h"
#include "tasks.h"

//Model

#include "train.h"
#include "sun.h"
#include "model.h"
#include "sensorIR.h"
#include "railChange.h"
#include "trafficLight.h"
#include "crossingGate.h"
#include "tracker.h"
#include "railway.h"
#include "anticollision.h"
#include "screen.h"
#include "semaphore.h"
#include "ctrlIlumination.h"
// Dummy function to catch signals

i2chandler_t* i2chandler[I2C_BUSES];
static int n_i2c_handlers = 0;
void catch_signal () {}
/*
void initializeModel(void) {
	// TODO
	int i;

	for (i=0;i<4;i++)
	{
		//IRsensors[i] = sensorIR_new(i);
	}
}
*/
void initializeXenomaiEnv(void) {
	// Catch signals
	signal(SIGTERM, catch_signal);
	signal(SIGINT, catch_signal);

	/* Avoids memory swapping for this program */
	mlockall(MCL_CURRENT | MCL_FUTURE);

	// Initialize rdtk to use rt_printf
	rt_print_auto_init(1);
}

void initializeWiringPi(void) {

	wiringPiSetup();

	// Replace with proper I2C modules load on kernel
	//system("gpio load i2c");

	//pinModes ....
}

void i2chandler_init(i2chandler_t* this, int n_bus) {
	
	this->i2chandler = i2c_open(n_bus);
	rt_mutex_create(&this->mutex, NULL);
	rt_printf("mutex ok\n");

}

i2chandler_t*
i2chandler_new(int n_bus) 
{
	i2chandler_t* this = (i2chandler_t*) malloc(sizeof(i2chandler_t));
	i2chandler_init(this, n_bus);
	i2chandler[n_i2c_handlers++] = this;
	
	return this;
}


void initializei2c(void) {

	//int i;
	i2chandler_new(0);
	i2chandler_new(0);
	//for(i = 0 ; i < I2C_BUSES ; i++)
	//	i2chandler_new(i);	
	
}

int main(int argc, char* argv[]) {

	// Initialize Xenomai RT enviroment
	initializeXenomaiEnv();

	// Initialize wiringPi lib, configure IO
	initializeWiringPi();
	
	// Initialize the model
	//initializeModel();
	
	//Initialize the i2c
	//initializei2c();
	// Initialize the train controller
	//trainCtrl_init();

	/*
	 * Arguments: &task,
	 * name,
	 * stack size (0=default),
	 * priority,
	 * mode (FPU, start suspended, ...)
	 */
	//rt_task_create(&task_dcc, "dccSend", 0, TASK_DCC_PRIORITY, 0);
	//rt_task_create(&task_poll, "polling", 0, TASK_POLL_PRIORITY, 0);
	//rt_task_create(&task_sun, "sun", 0, TASK_SUN_PRIORITY, 0);
	/*
	 * Arguments: &task,
	 * task function,
	 * function argument*/
	IRsensors_setup();
	rt_printf("ir ok \n");
	/*trains_setup();
	rt_printf("train ok \n");
	railways_setup();
	rt_printf("rail ok \n");
	semaphore_setup();
	rt_printf("semaphore ok \n");
	trafficLight_setup();
	rt_printf("tl ok \n");
	crossingGate_setup();
	rt_printf("cg ok \n");
	railChange_setup();
	rt_printf("rc ok \n");
	sun_setup();
	rt_printf("sun ok \n");
	model_init();
	rt_printf("model ok \n");
	screen_setup();
	rt_printf("screen ok \n");
	anticollision_setup();
	rt_printf("anti ok \n");
	tracker_init();
	rt_printf("tracker ok \n");
	ctrlilumination_init();
	rt_printf("ctrl ok \n");*/
	// TODO Hay que darle argumentos a la tarea!
	//rt_task_start(&task_dcc, &dcc_send, NULL );
	//rt_task_start(&task_poll, &daemon_poll_sensors, IRsensors);
	// rt_task_start(&task_sun, &daemon_update_sun, NULL );
	task_start_all();
	interp_run();

	// Remove the permanent tasks
	//rt_task_delete(&task_poll);
	//rt_task_delete(&task_dcc);
	//rt_task_delete(&task_sun);
	task_delete_all();
	return 0;
}
