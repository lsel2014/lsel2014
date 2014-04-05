#include <stdio.h>
#include <signal.h>
#include <unistd.h>

// Xenomai native skin libraries
#include <sys/mman.h>
#include <native/task.h>
#include <native/timer.h>

// RT compilant print library
#include <rtdk.h>

// Tasks
#include "Tasks/trainCtrl.h"
#include "Tasks/poll.h"
#include "Tasks/sun.h"

// Interpreter
#include "Interpreter/interp.h"

#include "daemon.h"

// Dummy function to catch signals
void catch_signal () {}

void initializeModel(void) {
	// TODO
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

void initializeInterpreter(void) {
	// TODO Add all the commands
	// interp_addcmd(...
}

int main(int argc, char* argv[]) {
	RT_TASK task_poll, task_trainctrl, task_sun, task_interpreter;

	// Initialize the model
	initializeModel();

	// Initialize Xenomai RT enviroment
	initializeXenomaiEnv();

	// Initialize wiringPi lib, configure IO
	initializeWiringPi();

	// Initialize the train controller
	trainCtrl_init();

	// Configure the command line interpreter
	initializeInterpreter();

	/*
	 * Arguments: &task,
	 * name,
	 * stack size (0=default),
	 * priority,
	 * mode (FPU, start suspended, ...)
	 */
	rt_task_create(&task_trainctrl, "trainCtrl", 0, TASK_TRAINCTRL_PRIORITY, 0);
	rt_task_create(&task_poll, "polling", 0, TASK_POLL_PRIORITY, 0);
	rt_task_create(&task_sun, "sun", 0, TASK_SUN_PRIORITY, 0);
	rt_task_create(&task_interpreter, "interpreter", 0, TASK_INTERPRETER_PRIORITY, 0);
	/*
	 * Arguments: &task,
	 * task function,
	 * function argument*/
	rt_task_start(&task_trainctrl, &trainCtrl_periodic, NULL );
	rt_task_start(&task_poll, &daemon_poll_sensors, NULL );
	rt_task_start(&task_sun, &daemon_update_sun, NULL );
	rt_task_start(&task_interpreter, &interp_run, NULL );



	// Wait to the interpreter to end
	rt_task_join(&task_interpreter);

	// Remove the permanent tasks
	rt_task_delete(&task_poll);
	rt_task_delete(&task_trainctrl);
	rt_task_delete(&task_sun);
	return 0;
}
