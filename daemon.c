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
#include "i2c1.h"
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
}

void i2chandler_init(i2chandler_t* this, int n_bus) {
	
	this->i2chandler = i2c_open(n_bus);
	rt_mutex_create(&this->mutex, NULL);

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

	int i;
	//i2chandler_new(0);
	for(i = 0 ; i < I2C_BUSES ; i++)
		i2chandler_new(i);	
	
}

int main(int argc, char* argv[]) {

	// Initialize Xenomai RT enviroment
	initializeXenomaiEnv();

	// Initialize wiringPi lib, configure IO
	initializeWiringPi();
	
	//Initialize the i2c
	i2c_pins_setup();
	initializei2c();

	IRsensors_setup();
	trains_setup();
	railways_setup();
	semaphore_setup();
	trafficLight_setup();
	crossingGate_setup();
	railChange_setup();
	sun_setup();
	model_init();
	//screen_setup();
	anticollision_setup();
	tracker_init();
	ctrlilumination_init();

	task_start_all();
	interp_run();

	task_delete_all();
	digitalWrite(19,0);
	digitalWrite(20,0);
	return 0;
}
