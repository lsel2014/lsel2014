#include <stdlib.h>
#include <native/mutex.h>
#include <stdbool.h>
#include <wiringPi.h>
#include <softPwm.h>

#include "tasks.h"
#include "crossingGate.h"
#include "interp.h"

crossingGate_t* crossingGates[MAXCROSINGGATES];
int n_crossingGates;
void 
setup_crossingGate(void) 
{
    int sensec[] = {2,3};
    n_crossingGates=0;
	crossingGates[0] = crossingGate_new(0, 11,sensec);
	interp_addcmd("barrier", crossingGate_cmd, "Set barrier state\n");
	int i;
}

int 
crossingGate_cmd(char* arg) 
{
	if (0 == strcmp(arg, "list")) {
		printf("Gate\t%s\r\n", (crossingGates[0]->position) == UP ? "UP" : "DOWN");

		return 0;
	}
	if (0 == strncmp(arg, "set ", strlen("set "))) {
		int state;
		state = atoi(arg + strlen("set "));
		crossingGate_set_position(crossingGates[0], state == 1 ? UP : DOWN);
		return 1;
	}
	return 1;
}

crossingGate_t*
crossingGate_new(int id, int GPIOline,int sensiblesectors[2]) {
	crossingGate_t* this = (crossingGate_t*) malloc(sizeof(crossingGate_t));
	crossingGate_init(this, id, GPIOline, DOWN, sensiblesectors);
    if (n_crossingGates < MAXCROSINGGATES) {
		crossingGates[n_crossingGates++] = this;
	}
	return this;
}

void 
crossingGate_init(crossingGate_t* this, int id, int GPIOline,
		                          position_t position, int sensiblesectors[2]) {
	int i;

	this->GPIOline = GPIOline;
	this->position = position;
	this->needsService = 0;
    this->sensiblesectors[0] = sensiblesectors[0];
    this->sensiblesectors[1] = sensiblesectors[1];
	pinMode(GPIOline, OUTPUT);
	softPwmCreate(GPIOline, 0, 200);

	rt_mutex_create(&this->mutex, NULL);
	task_add("Gate control", GATE_DEADLINE, crossingGate_move_task, this);
	crossingGate_set_position(this, position);

}

void 
crossingGate_destroy(crossingGate_t* this) 
{
	//This should be done properly
	free(this);
}
int*
crossingGate_get_sensiblesector(crossingGate_t* this)
{
   return this->sensiblesectors;                                  
}

void 
crossingGate_set_position(crossingGate_t* this, position_t position) 
{
	rt_mutex_acquire(&(this->mutex), TM_INFINITE);
	if (this->position != position) {
		this->position = position;
		this->needsService = 1;
	}
	rt_mutex_release(&this->mutex);
}

void 
crossingGate_move_task(void *args) 
{
	crossingGate_t* this = (crossingGate_t*) args;
	rt_task_set_periodic(NULL, TM_NOW, GATE_PERIOD);
	while (1) {
		rt_task_wait_period(NULL);
		if (this->needsService) {
			rt_mutex_acquire(&(this->mutex), TM_INFINITE);

			if (this->position == DOWN) {
				softPwmWrite(this->GPIOline, 18);
			} else {
				softPwmWrite(this->GPIOline, 9);
			}
			this->needsService = 0;
			rt_mutex_release(&this->mutex);
		} else {
			softPwmWrite(this->GPIOline, 0);
		}
	}
}
