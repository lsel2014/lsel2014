#include <stdio.h>
#include <signal.h>
#include <unistd.h>

// Xenomai native skin libraries
#include <sys/mman.h>
#include <native/task.h>
#include <native/timer.h>
// WiringPi
#include <wiringPi.h>

// RT compilant print library
#include <rtdk.h>

// Tasks
#include "dcc.h"
//#include "poll.h"
//#include "sunTasks.h"

// Interpreter
#include "Interpreter/interp.h"
#include "daemon.h"
#include "tasks.h"

//Model
#include "train.h"
#include "sun.h"
#include "model.h"
#include "Model/Sensors/sensorIR.h"
#include "Model/Actuators/railChange.h"
#include "Model/Actuators/trafficLight.h"
#include "Model/Actuators/crossingGate.h"

// Dummy function to catch signals
void catch_signal () {}

void initializeModel(void) {
	// TODO
	int i;
	
	for (i=0;i<4;i++)
	{
		//IRsensors[i] = sensorIR_new(i);
	}
}

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

int main(int argc, char* argv[]) {

	// Initialize Xenomai RT enviroment
	initializeXenomaiEnv();

	// Initialize wiringPi lib, configure IO
	initializeWiringPi();
	
	// Initialize the model
	initializeModel();

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
	//IRsensors_setup();
	printf("U JELLY?");
	model_init();
	trains_setup();
	setupRailChange();
	sun_setup();
	setup_trafficLight();
	setup_crossingGate();
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
