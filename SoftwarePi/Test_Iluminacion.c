#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define BUFF_INIT_ADDR 50
#define BUFF_ADDR 200




int main(void) {

	srand(time(NULL));

	int v1;
	int v2;
	int v3;

	char *resultados[4];
	v1 = random() % 30;
	v2 = random() % 12+1;
	v3 = random() % 30 + 1995;

	char buffer [50];
	sprintf (buffer, "%d/%d/%d", v1, v2, v3);

	printf ("%s %s \n","Se va a simular la fecha: ", buffer);


	// resultados = xmlDownloader (buffer);

	printf ("%s %s \n","Hora del amanecer: ", resultados[0] );
	printf ("%s %s \n","Hora del anochecer: ", resultados[1] );
	printf ("%s %s \n","Tiempo simulado: ", resultados[2] );
	printf ("%s %s \n","Tiempo real simulado: ", resultados[3] );


	return 0;
}
