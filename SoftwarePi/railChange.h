/*
 LSEL 2014

 @authors _____, J. Martin
 @date April 2014
*/

#ifndef railChange_H
#define railChange_H
#include <stdlib.h>
#include "interp.h"
typedef enum {
	LEFT, RIGHT
} direction_t;

typedef struct railChange_t {
	int GPIOline;
	direction_t direction;

	pthread_mutex_t mutex;

} railChange_t;

//----------------------------
void setupRailChange(void);
int railChange_cmd(char* arg);

void railChange_init(railChange_t* this, int GPIOline, direction_t direction);

railChange_t* railChange_new(int GPIOline, direction_t direction);

direction_t railChange_get_direction();

void railChange_set_direction(railChange_t* this, direction_t direction);

#endif

/*
  Local variables:
    mode: c
    c-file-style: stroustrup
  End:
*/
