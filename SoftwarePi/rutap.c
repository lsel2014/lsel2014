#include <stdio.h>
#include <wiringPi.h>
#include <native/task.h>
#include <rtdk.h>
#include <stdint.h>
#include <ncurses.h>
#include "controlvelocidad.h"
#include "trama.h"
#include "main_VelBar.h"


FILE *fr;            
char filename[20];
char line[7];
int sectortmp;
int velocidadtmp;
int direcciontmp;
int terminatmp = 0;
int cambioviatmp;
int salidaSector = 1;
int rutaActiva = 0;



int digit_to_int(char d)
{
 char str[2];

 str[0] = d;
 str[1] = '\0';
 return (int) strtol(str, NULL, 10);
}



void cargaRutaSiguiente()
{

    fgets (line, 6, fr);



 	sectortmp = line[1];
	velocidadtmp = digit_to_int(line[2])*10 + digit_to_int(line[3]);
	diesel_t.velocidad = velocidadtmp;
	direcciontmp = line[4];
	diesel_t.sentido = direcciontmp;
	cambioviatmp = digit_to_int(line[5]);
	terminatmp = digit_to_int(line[0]);
	//mvprintw(17, 3, "sector: %c velocidad: %d  direccion: %c cambioVia: %d termina: %d\n", diesel_t.current_sector, diesel_t.velocidad, cambioviatmp, diesel_t.sentido, terminatmp);
	    fgets (line, 6, fr);


}

void iniciaRuta()
{
	fr = fopen("ruta2.dta", "r");  
	cargaRutaSiguiente();
	rutaActiva = 1;
}





void paraTren()
{
	setVelocidad('1', '1','0');
}


void miraPosicion()
{	
	rt_task_set_periodic(NULL, TM_NOW, 100000000);
	while(1)
	{
		rt_task_wait_period(NULL);
		if(terminatmp == 0 && rutaActiva == 1)
		{
			mvprintw(17, 3, "sector: %c velocidad: %d  direccion: %c cambioVia: %d", diesel_t.current_sector, diesel_t.velocidad, diesel_t.sentido, cambioviatmp);
			if(salidaSector == 1 && (diesel_t.current_sector == sectortmp))
			{
				setVelocidad(1, direcciontmp ,(char)velocidadtmp);
				
				cambiarVia(cambioviatmp);
				cargaRutaSiguiente();
				salidaSector = 0;
				//printf("sector: %d velocidad: %d  direccion: %d cambioVia: %d termina: %d\n",
				 //sectortmp, velocidadtmp, cambioviatmp,  direcciontmp, terminatmp);

			}
			else if (salidaSector == 0 && diesel_t.current_sector != sectortmp) 
			{
				salidaSector = 1;
			}
		}
		else if(terminatmp == 1 && rutaActiva == 1)
		{
			paraTren();
			rutaActiva = 0;
			fclose(fr);
		}
	
	}

}


