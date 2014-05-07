#ifndef CROSSINGGATE_H
#define CROSSINGGATE_H

#include "observer.h"
#include "train.h"

#define GATE_PERIOD 500000000
#define GATE_DEADLINE 500000000
typedef enum {
	UP, DOWN
} position_t;

typedef struct crossingGate_t {
	//Inheritance
	observer_t observer;
	//Attributes
	int GPIOline;
	position_t position;
	char sectorCrossing;
	int needsService;
	//Private
	char trainsInside[MAXTRAINS];
	int trainsInsideIndex;
	pthread_mutex_t mutex;

} crossingGate_t;

void setup_crossingGate(void);
int crossingGate_cmd(char* arg);
/*
 * Object creation / destruction
 */
crossingGate_t* crossingGate_new(int id, int GPIOline, char sectorCrossing);
void crossingGate_init(crossingGate_t* this, int id, int GPIOline,
		char sectorCrossing, position_t position);
void crossingGate_destroy(crossingGate_t* this);

/*
 * Getters / setters
 */
position_t crossingGate_get_position(crossingGate_t* this);
void crossingGate_set_position(crossingGate_t* this, position_t position);
void crossingGate_move_task(void *args);
char crossingGate_get_sectorCrossing(crossingGate_t* this);
void crossingGate_set_sectorCrossing(crossingGate_t* this, char sectorCrossing);

//Checks if a certain train is inside the crossing gate sector
int crossingGate_getIDIndex(crossingGate_t* this, char trainID);

/*
 * Observer function
 *
 * Crossing gate behaviour belongs here
 */
void crossingGate_notify(observer_t* this, observable_t* observable);

#endif
