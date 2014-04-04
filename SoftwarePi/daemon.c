#include <stdio.h>
#include <signal.h>
#include <unistd.h>

// Xenomai native skin libraries
#include <sys/mman.h>
#include <native/task.h>
#include <native/timer.h>

//RT compilant print library
#include <rtdk.h>

//Tasks
#include "Tasks/trainCtrl.h"
#include "Tasks/poll.h"
#include "Tasks/sun.h"


void catch_signal () {}

void initializeModel(void) {

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
	//variableInit();

	wiringPiSetup();

	// Replace with proper I2C modules load on kernel
	//system("gpio load i2c");

	//pinModes ....
}



int main(int argc, char* argv[]) {
	RT_TASK task_poll, task_trainctrl, task_sun;

	// Initialize the model
	initializeModel();

	// Initialize Xenomai RT enviroment
	initializeXenomaiEnv();

	// Initialize wiringPi lib, configure IO
	initializeWiringPi();

	// Initialize the train controller
	trainCtrl_init();

	/*
	 * Arguments: &task,
	 * name,
	 * stack size (0=default),
	 * priority,
	 * mode (FPU, start suspended, ...)
	 */
	rt_task_create(&task_poll, "polling", 0, 50, 0);
	rt_task_create(&task_trainctrl, "trainCtrl", 0, 55, 0);
	rt_task_create(&task_sun, "sun", 0, 20, 0);
	/*
	 * Arguments: &task,
	 * task function,
	 * function argument*/
	rt_task_start(&task_poll, &daemon_poll_sensors, NULL );
	rt_task_start(&task_trainctrl, &trainCtrl_periodic, NULL );
	rt_task_start(&task_sun, &daemon_update_sun, NULL );

	pause();  //REMOVE, replace by a join to the socket thread
	rt_task_delete(&task_poll);
	rt_task_delete(&task_trainctrl);
	rt_task_delete(&task_sun);
	return 0;
}
