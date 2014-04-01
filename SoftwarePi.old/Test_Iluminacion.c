#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "xmlParser.c"


int main(void) {

	srand(time(NULL));

	int v1;
	int v2;
	int v3;

	char **results;

	v1 = random() % 30;
	v2 = random() % 12+1;
	v3 = random() % 30 + 1995;

	char buffer [9];
	buffer[8] = '\0';
	sprintf (buffer, "%d/%d/%d", v1, v2, v3);

	printf ("Se va a simular la fecha: %s\n", buffer);

	results = xmlParser(buffer);

	printf ("Hora del amanecer: %s\n", results[0]);
	printf ("Hora del anochecer: %s\n", results[1]);
	//printf ("%s %s \n","Tiempo simulado: ", resultados[2] );
	//printf ("%s %s \n","Tiempo real simulado: ", resultados[3] );


	return 0;
}
