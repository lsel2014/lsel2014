#include "sun.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void testSun() {

	srand(time(NULL));

	int v1, v2, v3, v4;

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
	
	printf ("Se va a simular la fecha: %s\n", buffer);

	sun_t* sun = sun_new (buffer, "00:00:00", 1, 1);
	printf ("Amanecer: %s \nAnochecer: %s \n", sun->sunrise, sun->sunset);

	sun_destroy(sun);

}

int main (int argc, char** argv) {

	testSun();
	return 0;

}
