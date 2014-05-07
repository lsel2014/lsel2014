#include <stdlib.h>

#include <native/task.h>

#include <native/mutex.h>
#include <stdbool.h>
#include "../../tasks.h"
#include "crossingGate.h"
#include <wiringPi.h>
#include <softPwm.h>

static crossingGate_t* barrier;

void setup_crossingGate(void) {
	barrier = crossingGate_new(0, 11, 2);
	interp_addcmd("barrier", crossingGate_cmd, "Set barrier state\n");
	int i;
	for (i = 0; i < ntrains; i++) {
		observable_register_observer(&(trains[i]->observable),
				(observer_t*) barrier);
	}
}

int crossingGate_cmd(char* arg) {
	if (0 == strcmp(arg, "list")) {
		printf("Gate\t%s\r\n", (barrier->position) == UP ? "UP" : "DOWN");

		return 0;
	}
	if (0 == strncmp(arg, "set ", strlen("set "))) {
		int state;
		state = atoi(arg + strlen("set "));
		crossingGate_set_position(barrier, state == 1 ? UP : DOWN);
		return 1;
	}
	return 1;
}

crossingGate_t*
crossingGate_new(int id, int GPIOline, char sectorCrossing) {
	crossingGate_t* this = (crossingGate_t*) malloc(sizeof(crossingGate_t));
	crossingGate_init(this, id, GPIOline, sectorCrossing, DOWN);

	return this;
}

void crossingGate_init(crossingGate_t* this, int id, int GPIOline,
		char sectorCrossing, position_t position) {
	int i;

	observer_init((observer_t*) this, crossingGate_notify);
	this->GPIOline = GPIOline;
	this->sectorCrossing = sectorCrossing;
	this->position = position;
	this->needsService = 0;
	for (i = 0; i < MAXTRAINS; i++) {
		this->trainsInside[i] = 0;
	}
	this->trainsInsideIndex = 0;

	pinMode(GPIOline, OUTPUT);
	softPwmCreate(GPIOline, 0, 200);

	rt_mutex_create(&this->mutex, NULL);
	task_add("Gate control", GATE_DEADLINE, crossingGate_move_task, this);
	crossingGate_set_position(this, position);

}

void crossingGate_destroy(crossingGate_t* this) {
	//This should be done properly
	free(this);
}

position_t crossingGate_get_position(crossingGate_t* this) {
	return this->position;
}

void crossingGate_set_position(crossingGate_t* this, position_t position) {
	rt_mutex_acquire(&(this->mutex), TM_INFINITE);
	if (this->position != position) {
		this->position = position;
		this->needsService = 1;
	}
	rt_mutex_release(&this->mutex);
}

char crossingGate_get_sectorCrossing(crossingGate_t* this) {
	return this->sectorCrossing;
}

void crossingGate_set_sectorCrossing(crossingGate_t* this, char sectorCrossing) {
	rt_mutex_acquire(&(this->mutex), TM_INFINITE);

	this->sectorCrossing = sectorCrossing;

	rt_mutex_release(&this->mutex);
}

void crossingGate_move_task(void *args) {
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

int crossingGate_getIDIndex(crossingGate_t* this, char trainID) {
	int r = -1;
	int i;

	for (i = 0; i < MAXTRAINS; i++) {
		if (this->trainsInside[i] == trainID) {
			r = i;
		}
	}

	return r; //returns the array's index for that ID, or -1 if the ID was not in the list
}

void crossingGate_notify(observer_t* this, observable_t* observable) {
	train_t* train = (train_t*) observable;
	crossingGate_t* thisCG = (crossingGate_t*) this;
	//telemetry_t telemetry = train->telemetry;

	char sector = train->telemetry->sector;

	//Check if a train has just entered the crossing gate sector
	if (sector == thisCG->sectorCrossing) {

		//Put that train inside the list, increment the index and lower the barrier
		thisCG->trainsInside[thisCG->trainsInsideIndex++] = train->ID;
		crossingGate_set_position(thisCG, DOWN);

	} else { //The train is not in the the crossing gate sector

		int position = crossingGate_getIDIndex(thisCG, train->ID);
		int i;

		//Check if the train ID is in the list
		//If so, it means the train has just left the crossing gate sector
		if (position >= 0) {

			//Erase that train ID, and fill the gap
			for (i = position; i < MAXTRAINS - 1; i++) {
				thisCG->trainsInside[i] = thisCG->trainsInside[i + 1];
			}

			//Decrement the index. If there are no trains left, rise the barrier
			if (--thisCG->trainsInsideIndex == 0)
				crossingGate_set_position(thisCG, UP);
		}
	}

}
