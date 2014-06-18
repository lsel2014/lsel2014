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
	//rt_printf("I2C \n");
	IRsensors_setup();
	//rt_printf("IR \n");
	trains_setup();
	//rt_printf("train \n");
	railways_setup();
	//rt_printf("rail\n");
	semaphore_setup();
	//rt_printf("sem \n");
	trafficLight_setup();
	//rt_printf("TL\n");
	crossingGate_setup();
	//rt_printf("CG \n");
	railChange_setup();
	//rt_printf("RC \n");
	//sun_setup();
	//rt_printf("sun \n");
	model_init();
	//rt_printf("model \n");
	screen_setup();
	anticollision_setup();
	tracker_init();
	ctrlilumination_init();

	task_start_all();
	interp_run();

	task_delete_all();
	i2c_close(i2chandler[0]->i2chandler );
	i2c_close(i2chandler[1]->i2chandler );
	digitalWrite(19,0);
	digitalWrite(20,0);
	return 0;
}
