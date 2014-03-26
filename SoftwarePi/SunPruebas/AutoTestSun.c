#include "sun.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define COLOR_RED     		"\x1b[31m"
#define COLOR_GREEN		"\x1b[32m"
#define COLOR_RESET   		"\x1b[0m"

int testSun() {

	srand(time(NULL));

	int v1, v2, v3, v4;
	int compare;
	int successes, result;

	v1 = (random() % 27)+1;
	v2 = (random() % 11)+1;
	v3 = (random() % 14)+2000;
	v4 = 0;

	char buffer [11];
	buffer[10] = '\0';

	if (v1<10 && v2<10) {
		sprintf (buffer, "%d%d/%d%d/%d",v4, v1, v4, v2, v3);
	}
	else if (v1<10) {
		sprintf (buffer, "%d%d/%d/%d", v4, v1, v2, v3);
        }
	else if (v2<10) {
		sprintf (buffer, "%d/%d%d/%d", v1, v4, v2, v3);
	}
	else{
		sprintf (buffer, "%d/%d/%d", v1, v2, v3);
        }
	
	//printf ("Se va a simular la fecha: %s\n", buffer);
	sun_t* sun = sun_new (buffer, "00:00:00", 1, 1);
	
	printf ("Amanecer: %s\t->\tHoras ", sun->sunrise);
	compare = dateToInt(sun,0,0);
	if (compare < 0 || compare > 23) {
		printf(COLOR_RED"NOK"COLOR_RESET);
	} else {
		printf(COLOR_GREEN"OK!"COLOR_RESET);
		successes++;
	}

	printf("\tMinutos ");
	compare = dateToInt(sun,1,0);
	if (compare < 0 || compare > 59) {
		printf(COLOR_RED"NOK"COLOR_RESET);
	} else {
		printf(COLOR_GREEN"OK!"COLOR_RESET);
		successes++;
	}

	printf("\tSegundos ");
	compare = dateToInt(sun,2,0);
	if (compare < 0 || compare > 59) {
		printf(COLOR_RED"NOK\n"COLOR_RESET);
	} else {
		printf(COLOR_GREEN"OK!\n"COLOR_RESET);
		successes++;
	}

	printf ("Anochecer: %s\t->\tHoras ", sun->sunset);
	compare = dateToInt(sun,0,1);
	if (compare < 0 || compare > 23) {
		printf(COLOR_RED"NOK"COLOR_RESET);
	} else {
		printf(COLOR_GREEN"OK!"COLOR_RESET);
		successes++;
	}

	printf("\tMinutos ");
	compare = dateToInt(sun,1,1);
	if (compare < 0 || compare > 59) {
		printf(COLOR_RED"NOK"COLOR_RESET);
	} else {
		printf(COLOR_GREEN"OK!"COLOR_RESET);
		successes++;
	}

	printf("\tSegundos ");
	compare = dateToInt(sun,2,1);
	if (compare < 0 || compare > 59) {
		printf(COLOR_RED"NOK\n"COLOR_RESET);
	} else {
		printf(COLOR_GREEN"OK!\n"COLOR_RESET);
		successes++;
	}

	if (successess != 6) {
		result=0;
	} else {
		result=1;
	}

	sun_destroy(sun);
	return result;

}

int main (int argc, char** argv) {
	
	int r;

	r = testSun();
	
	if (r == 0) {
		printf(COLOR_RED"TEST NOK\n"COLOR_RESET);
	} else {
		printf(COLOR_GREEN"TEST OK!\n"COLOR_RESET);
	}
	
	return 0;

}
