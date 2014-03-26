#include "sun.h"
#include <stdio.h>
#include <stdlib.h>

int main (int argc, char** argv)
{
	if (argc != 2){
		printf("Debes introducir una fecha correcta: dd/mm/yyyy\n");
		exit (1);
	}
	sun_t* sun = sun_new (argv[1], "00:00:00", 1, 1);
	printf ("Se va a simular el día %s\n", argv[1]);
	printf ("Amanecer: %s \nAnochecer: %s \n", sun->sunrise, sun->sunset);
	//printf ("Hora: %d ", dateToInt(sun, 0, 0));
	//printf ("Minutos: %d ", dateToInt(sun, 1, 0));
	//printf ("Segundos: %d ", dateToInt(sun, 2, 0));
	//printf ("Hora: %d ", dateToInt(sun, 0, 1));
	//printf ("Minutos: %d ", dateToInt(sun, 1, 1));
	//printf ("Segundos: %d \n", dateToInt(sun, 2, 1));
	sendHour(sun);
	sun_destroy(sun);
	exit (0);
}
