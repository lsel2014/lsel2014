#ifndef CROSSINGGATE_H
#define CROSSINGGATE_H

#include "observer.h"
#include "train.h"

#define GATE_PERIOD 500000000
#define GATE_DEADLINE 500000000
#define MAXCROSINGGATES 4

typedef enum {
	UP, DOWN
} position_t;

typedef struct crossingGate_t {
	//Inheritance
	observable_t observable; 
	//Attributes
	//int GPIOline;
	position_t position;
	int id;
	int needsService;
	//i2c
	uint16_t i2c_address;
	//Private
	//int sensiblesectors[2];
	int sensiblesector;
	RT_MUTEX mutex;

} crossingGate_t;

extern crossingGate_t* crossingGates[MAXCROSINGGATES];

void crossingGate_setup(void);
int crossingGate_cmd(char* arg);
/*
 * Object creation / destruction
 */
/*crossingGate_t* crossingGate_new(int id, int GPIOline, char sectorCrossing);
void crossingGate_init(crossingGate_t* this, int id, int GPIOline,
		char sectorCrossing, position_t position);*/
crossingGate_t* crossingGate_new(int id,/* int sensiblesectors[2],*/int sensiblesector, uint16_t i2c_address);

void crossingGate_init(crossingGate_t* this, int id, position_t position,
                                       /*int sensiblesectors[2],*/ int sensiblesector, uint16_t i2c_address);
/*crossingGate_t* crossingGate_new(int id, int GPIOline, int sensiblesectors[2]);

void crossingGate_init(crossingGate_t* this, int id, int GPIOline,
		                           position_t position,int sensiblesectors[2]);	*/	                           
void crossingGate_destroy(crossingGate_t* this);

/*
 * Getters / setters
 */
position_t crossingGate_get_position(crossingGate_t* this);
void crossingGate_set_position(crossingGate_t* this, position_t position);
void crossingGate_set_light(crossingGate_t* this, int state);
void crossingGate_move_task(void *args);
int crossingGate_get_sensiblesector(crossingGate_t* this);

#endif
