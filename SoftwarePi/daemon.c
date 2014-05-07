#include <stdio.h>
#include <signal.h>
#include <unistd.h>

// WiringPi
#include <wiringPi.h>

// Tasks
#include "dcc.h"
//#include "poll.h"
//#include "sunTasks.h"

// Interpreter
#include "interp.h"
#include "daemon.h"
#include "task.h"

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

#ifdef __XENO__
#include <rtdk.h>
#endif

// Dummy function to catch signals
void catch_signal() {
}

void initializeModel(void) {
	// TODO
	int i;

	for (i = 0; i < 4; i++) {
		//IRsensors[i] = sensorIR_new(i);
	}
}

void initializeXenomaiEnv(void)
{
	// Catch signals
	signal(SIGTERM, catch_signal);
	signal(SIGINT, catch_signal);
#ifdef __XENO__
	/* Avoids memory swapping for this program */
	mlockall(MCL_CURRENT | MCL_FUTURE);
	// Initialize rdtk to use rt_printf
	rt_print_auto_init(1);
#endif
}

void initializeWiringPi(void)
{
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
	IRsensors_setup();
	trains_setup();
	railways_setup();
	anticollision_setup();
	fbtft_init();
	model_init();

	tracker_init();

	//setupRailChange();
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
	fbtft_destroy();
	return 0;
}
