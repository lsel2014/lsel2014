/*
	LSEL 2014

	@authors _____, J. Martin
	@date April 2014

	Represents a electrical rail changer.
	
*/

#include "railChange.h"
#include <wiringPi.h> 

static railChange_t* changer;

void setupRailChange (void)
{
	changer = railChange_new(10, LEFT);
	interp_addcmd("changer", railChange_cmd, "Set rail state\n");
}

int railChange_cmd(char* arg) {
	if (0 == strcmp(arg, "list")) {
		printf("Rail changer\t%s\r\n", (changer->direction) == LEFT ? "LEFT (1) (OUTER)" : "RIGHT (2) (INNER)");
		
		return 0;
	}
	if (0 == strncmp(arg, "set ", strlen("set "))) {
		int state;
		state = atoi(arg + strlen("set "));
		railChange_set_direction (changer, state==1 ? LEFT : RIGHT);
		return 1;
	}
	return 1;
}

void
railChange_init (railChange_t* this, int GPIOline, direction_t direction)
{
	this->direction = direction;
	this->GPIOline = GPIOline;
	
	pinMode (GPIOline, OUTPUT);
	rt_mutex_create (&this->mutex, NULL);
	railChange_set_direction(this, direction);

}

railChange_t*
railChange_new (int GPIOline, direction_t direction)
{
	railChange_t* this = (railChange_t*) malloc (sizeof (railChange_t));
	railChange_init (this, GPIOline, direction);

	return this;

}


direction_t
railCange_get_direction (railChange_t* this)
{
	return this->direction;
}


void
railChange_set_direction (railChange_t* this, direction_t direction)
{
	rt_mutex_acquire(&(this->mutex), TM_INFINITE);
	
	this->direction = direction;
	digitalWrite (this->GPIOline, direction==LEFT ? 1 : 0);
	rt_mutex_release(&(this->mutex));
}
