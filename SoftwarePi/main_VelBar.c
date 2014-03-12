/*
* Name: IRtasks.c
* Description: Read some GPIO pins using wiringPi functions
* Authors: Group 1 LSEL2013
*/

// Libraries
#include <wiringPi.h>
#include <softPwm.h>
#include <stdlib.h>
#include <stdint.h>
#include <native/mutex.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/mman.h>
#include <native/task.h>
#include <native/timer.h>
#include <rtdk.h>
#include <ncurses.h>
#include <string.h>
#include <math.h>
#include "controlvelocidad.h"
#include "pin_config.h"

#include <time.h>

struct train {
char train_type;
char current_sector;
SRTIME current_time;
char prev_sector;
SRTIME prev_time;
int velocidad; //velocidad en codigo 0,4,8,12,16...
double velocidad_fis;
char sentido;
int dist_restante;
double tiempo_restante;
};



// Global variables
RT_TASK IRtracking_task, Actualiza_pantalla,controldeVelocidad,ruta;
RT_MUTEX mutex_steam, mutex_diesel, mutex_pVia;
int pin, err_steam, err_diesel, err_pVia, s_count, d_count;
struct train steam_t, diesel_t;
char pVia, trenSeleccionado;
char s0_state, s1_state, s2_state, s3_state;
RTIME s0_ticks_temp, s1_ticks_temp, s2_ticks_temp, s3_ticks_temp;
int velocidad = 0;
int trenActual = 1; //1 = diesel, 0 = vapor
char in[64];


FILE *fr;           
char filename[20];
char line[7];
char sectortmp;
int velocidadtmp;
char direcciontmp;
char terminatmp = '0';
int cambioviatmp;
//int salidaSector = 1;
//int rutaActiva = 0;
char primeraOrden = '0';
char salioEstacion = '0';
char rutasActiva = '0';


int digit_to_int(char d)
{
 char str[2];

 str[0] = d;
 str[1] = '\0';
 return (int) strtol(str, NULL, 10);
}



void cargaRutaSiguiente()
{
	
    	fgets (line, 7, fr);
 	sectortmp = line[1];
	velocidadtmp = digit_to_int(line[2])*10 + digit_to_int(line[3]);
	diesel_t.velocidad = velocidadtmp;
	direcciontmp = line[4];
	if (direcciontmp == '1'){
		diesel_t.sentido = FORWARD;
		//mvprintw(20, 3, "FORWARD");	
	}
	else{
		diesel_t.sentido = REVERSE;
		//mvprintw(20, 3, "REVERSE");
	}

	cambioviatmp = digit_to_int(line[5]);
	terminatmp = line[0];

	//mvprintw(18, 3, "lectura: %s\n", line);
	fgets (line, 7, fr);


}

void iniciaRuta()
{
	fr = fopen("ruta2.dta", "r");
	cargaRutaSiguiente();
	primeraOrden = '1';
	//rutaActiva = 1;
	//mvprintw(25,3,"Inicio de Ruta programada:");
	
}


void paraTren()
{
	setVelocidad('1', '1','0');
}

void cambiarVia(int i)
{	
	if(i == 2){
		if(pVia == '1')
 		{
	    		pVia = '0';
	   		digitalWrite (GPIO_CAMBIO_VIA,0) ; // Off
		}else{
    			pVia = '1';
			digitalWrite (GPIO_CAMBIO_VIA,1) ; // On
 		}
	}else{
		
		pVia = (char) i;
	   	digitalWrite (GPIO_CAMBIO_VIA,i);
	}
}

void miraPosicion()
{	
	rt_task_set_periodic(NULL, TM_NOW, 100000000);
	while(1)
	{
		rt_task_wait_period(NULL);
		/*if(terminatmp == 0 && rutaActiva == 1)
		{	
			
			//mvprintw(17, 3, "sector: %c velocidad: %d  direccion: %c cambioVia: %d\n", diesel_t.current_sector, 			diesel_t.velocidad, diesel_t.sentido, cambioviatmp);
			if(salidaSector == 1 && (diesel_t.current_sector == (char)sectortmp))
			{
				
				setVelocidad(1, diesel_t.sentido ,(char)velocidadtmp);
				fprintf(frw ,"S %c S_tmp %c\n", diesel_t.current_sector, (char)sectortmp);
				cambiarVia(cambioviatmp);
				cargaRutaSiguiente();
				salidaSector = 0;
				

			}
			else if (salidaSector == 0 && diesel_t.current_sector != (char)sectortmp) 
			{
				salidaSector = 1;
			}
		}
		else if(terminatmp == 1 && rutaActiva == 1)
		{
			paraTren();
			rutaActiva = 0;
			fclose(fr);
			fclose(frw);
		}

	*/
		
		if(primeraOrden == '1' && rutasActiva == '1')
		{
			
			if(fseek(fr, 0 , SEEK_SET) == 1)
			{
				//mvprintw(20, 3, "Error al posicionarse al princicio del fichero");
			}			
			cargaRutaSiguiente();
			setVelocidad(1, diesel_t.sentido ,(char)velocidadtmp);
			cambiarVia(cambioviatmp);
			if ((salioEstacion == '0') && (diesel_t.current_sector !='0'))
			{
				salioEstacion = '1';
				primeraOrden = '0';
			}
		}else if (rutasActiva == '1')
		{
			if (salioEstacion && (diesel_t.current_sector == sectortmp))
			{	
				cargaRutaSiguiente();
				setVelocidad(1, diesel_t.sentido ,(char)velocidadtmp);
				cambiarVia(cambioviatmp);		
			}
				
			else if (terminatmp == '1')
			{	
				
				rutasActiva = '0';
				mvprintw(26, 3, "Sistema de rutas inactivo");
				paraTren ();
				fclose(fr);
				salioEstacion = '0';
				primeraOrden = '1';	
			}
		}
	}
}

void CalculoTiempoRestante_sector (struct train *tren){
	

        float dist_restante;
	//float velocidad_local = dameVelocidadFisica(tren.velocidad);
	

	
	if ( ((*tren).sentido == FORWARD) && ((*tren).velocidad != 0)){
		if ((*tren).current_sector == '0'){
			dist_restante = VUELTA_COMPLETA;
		} else if ((*tren).current_sector == '1'){
			dist_restante = VUELTA_123;
		} else if ((*tren).current_sector == '2'){
			dist_restante = VUELTA_23;
		} else if ((*tren).current_sector == '3'){
			dist_restante = VUELTA_3;
		}

		if ((*tren).velocidad_fis < 0) mvprintw(15,5,"Velocidad negativa! No puedo poner el timer");
		
		else{
			(*tren).tiempo_restante = ceil(dist_restante/(float) (*tren).velocidad_fis);
			(*tren).dist_restante = dist_restante;
				
			if( (*tren).train_type == TREN_GASOIL){
				mvprintw(TIMER_CUAD_UP+3,TIMER_CUAD_LEFT+16, "%3.0f seg         ", (*tren).tiempo_restante);
			
			}else{
				mvprintw(TIMER_CUAD_UP+3,TIMER_CUAD_LEFT+15+TIMER_CUAD_SPLIT, "%3.0f seg         ", (*tren).tiempo_restante);
			}	
		}	
	}

	else {
		if( (*tren).train_type == TREN_GASOIL){
			if ((*tren).current_sector == '0') mvprintw(TIMER_CUAD_UP+3,TIMER_CUAD_LEFT+16, "En la Estacion");
			else mvprintw(TIMER_CUAD_UP+3,TIMER_CUAD_LEFT+16, "Tren Retrasado");
			
		}else{
			if ((*tren).current_sector == '0') mvprintw(TIMER_CUAD_UP+3,TIMER_CUAD_LEFT+15+TIMER_CUAD_SPLIT, "En la Estacion");
			else mvprintw(TIMER_CUAD_UP+3,TIMER_CUAD_LEFT+15+TIMER_CUAD_SPLIT, "Tren Retrasado");
		}
	}
	return;
	
}

	
// IRtracking function
void IRtracking(void *arg)
{
  //RT_TASK *curtask;			// curtask y curtaskinfo se utilizan para heredar la información de la tarea actual
  //RT_TASK_INFO curtaskinfo;	// para posteriormente indicar qué tarea se está ejecutando, etc. (ver función rt_task_inquire al final de la tarea)
  //RTIME ticks_temp;

  rt_task_set_periodic(NULL, TM_NOW, IRT_PERIOD); 
    
  while(1){
    struct train diesel_c, steam_c;

    // Blocks the while loop till the next periodic interruption
    rt_task_wait_period(NULL);

    // Acquire mutex_steam and mutex_diesel
    //rt_mutex_acquire(&mutex_steam,TM_INFINITE);
    rt_mutex_acquire(&mutex_diesel,TM_INFINITE);
    diesel_c = diesel_t;
    steam_c = steam_t;
    rt_mutex_release(&mutex_diesel);

    // SECTOR 0 ----------------------------------------------------------------------------------------------------------
    // 'Idle' state
    if (s0_state=='I'){
      if (digitalRead(GPIO_IR0_ALTO) == HIGH){   // Top sensor. HIGH = Steam train
	// Change to the 'Steam train' state and save the time information
	s0_state = 'S';
	s0_ticks_temp = rt_timer_read();
	if (steam_c.sentido == REVERSE){
	  digitalWrite(GPIO_SBARRIER, HIGH);
	  digitalWrite(GPIO_SEMAFORO, HIGH);
	}
      }
      else if (digitalRead(GPIO_IR0_BAJO) == HIGH){// Bottom sensor. HIGH = Diesel train or the first part of the Steam train.
	// Change to the 'Checking' state and save the time information
	s0_state = 'C';
	s0_ticks_temp = rt_timer_read();
	if (diesel_c.sentido == REVERSE){
	  digitalWrite(GPIO_SBARRIER, HIGH);
	  digitalWrite(GPIO_SEMAFORO, HIGH);
	}
      }
    }

    // 'Steam train' state
    else if (s0_state=='S'){
      if (digitalRead(GPIO_IR0_BAJO) == LOW){       // Bottom sensor. LOW = No train
	s_count++;            
	if (s_count == IDLE_COUNT) {
	  s_count = 0;
	  s0_state = 'I';
	  steam_c.prev_sector = steam_c.current_sector;
	  if (steam_c.sentido == FORWARD) steam_c.current_sector = '0';
	  else steam_c.current_sector = '3';

	  //CalculoTiempoRestante_sector (&steam_c);

	  steam_c.prev_time = steam_c.current_time;
	  steam_c.current_time = rt_timer_ticks2ns(s0_ticks_temp);
	}
      }
      else
	s_count = 0;
    }
    
    // 'Checking' state
    else if (s0_state=='C'){
        
      if (digitalRead(GPIO_IR0_ALTO) == HIGH){    // Top sensor. HIGH = Steam train
	// Change to the 'Steam train' state and save time information
	s0_state = 'S';
	s0_ticks_temp = rt_timer_read();
      }
        
      else if (digitalRead(GPIO_IR0_BAJO) == LOW){// Bottom sensor. LOW = Diesel train
	d_count++;            
	if (d_count == IDLE_COUNT){
	  d_count = 0;
	  s0_state = 'I';
	  diesel_c.prev_sector = diesel_c.current_sector;
	  if (diesel_c.sentido == FORWARD) diesel_c.current_sector = '0';
	  else diesel_c.current_sector = '3';
		
	  //CalculoTiempoRestante_sector (&diesel_c);

	  diesel_c.prev_time = diesel_c.current_time;
	  diesel_c.current_time = rt_timer_ticks2ns(s0_ticks_temp);
	}
      } else {
	d_count = 0;
      }
    }
 
    // END OF SECTOR 0 ---------------------------------------------------------------------------------------------------
    
    // SECTOR 1 ----------------------------------------------------------------------------------------------------------
    // 'Idle' state
    if (s1_state=='I'){

      if (digitalRead(GPIO_IR1_ALTO) == HIGH){   // Top sensor. HIGH = Steam train
	// Change to the 'Steam train' state and save the time information
	s1_state = 'S';
	s1_ticks_temp = rt_timer_read();
	if (steam_c.sentido == FORWARD){
	  digitalWrite(GPIO_SBARRIER, HIGH);
	  digitalWrite(GPIO_SEMAFORO, HIGH);
	}
      }
      else if (digitalRead(GPIO_IR1_BAJO) == HIGH){// Bottom sensor. HIGH = Diesel train or the first part of the Steam train.
	// Change to the 'Checking' state and save the time information
	s1_state = 'C';
	s1_ticks_temp = rt_timer_read();
	if (diesel_c.sentido == FORWARD){
	  digitalWrite(GPIO_SBARRIER, HIGH);
	  digitalWrite(GPIO_SEMAFORO, HIGH);
	}
      }
    }

    // 'Steam train' state
    else if (s1_state=='S'){
      if (digitalRead(GPIO_IR1_BAJO) == LOW){       // Bottom sensor. LOW = No train
	s_count++;            
	if (s_count == IDLE_COUNT) {
	  s_count = 0;
	  s1_state = 'I';
	  steam_c.prev_sector = steam_c.current_sector;
	  if (steam_c.sentido == FORWARD){
	    steam_c.current_sector = '1';
	  }
	  else steam_c.current_sector = '0';

	  //CalculoTiempoRestante_sector (&steam_c);

	  steam_c.prev_time = steam_c.current_time;
	  steam_c.current_time = rt_timer_ticks2ns(s1_ticks_temp);
	}
      }
      else
	s_count = 0;
    }
    
    // 'Checking' state
    else if (s1_state=='C'){
        
      if (digitalRead(GPIO_IR1_ALTO) == HIGH){    // Top sensor. HIGH = Steam train
	// Change to the 'Steam train' state and save time information
	s1_state = 'S';
	s1_ticks_temp = rt_timer_read();
      }
        
      else if (digitalRead(GPIO_IR1_BAJO) == LOW){// Bottom sensor. LOW = Diesel train
	d_count++;            
	if (d_count == IDLE_COUNT){
	  d_count = 0;
	  s1_state = 'I';
	  diesel_c.prev_sector = diesel_c.current_sector;
	  if (diesel_c.sentido == FORWARD) diesel_c.current_sector = '1';
	  else diesel_c.current_sector = '0';

	  //CalculoTiempoRestante_sector (&diesel_c);

	  diesel_c.prev_time = diesel_c.current_time;
	  diesel_c.current_time = rt_timer_ticks2ns(s1_ticks_temp);
	}
      } else {
	d_count = 0;
      }
    }
    // END OF SECTOR 1 ---------------------------------------------------------------------------------------------------  

    // SECTOR 2 ----------------------------------------------------------------------------------------------------------
    // 'Idle' state
    if (s2_state=='I'){
      if (digitalRead(GPIO_IR2_ALTO) == HIGH){   // Top sensor. HIGH = Steam train
	// Change to the 'Steam train' state and save the time information
	s2_state = 'S';
	s2_ticks_temp = rt_timer_read();
	if (steam_c.sentido == FORWARD) {
	  // Bajar barrera
	  softPwmWrite(GPIO_BARRIER_PWM, NINETY_DEGREE);
	  sleep(1);
	  softPwmWrite(GPIO_BARRIER_PWM, STOP_PWM);
	}
	else {
	  // Subir barrera
	  softPwmWrite(GPIO_BARRIER_PWM, ZERO_DEGREE);
	  sleep(1);
	  softPwmWrite(GPIO_BARRIER_PWM, STOP_PWM);
	  digitalWrite(GPIO_SBARRIER, LOW);
	  digitalWrite(GPIO_SEMAFORO, LOW);
	}
      }
      else if (digitalRead(GPIO_IR2_BAJO) == HIGH){// Bottom sensor. HIGH = Diesel train or the first part of the Steam train.
	// Change to the 'Checking' state and save the time information
	s2_state = 'C';
	s2_ticks_temp = rt_timer_read();
	if (diesel_c.sentido == FORWARD) {
	  // Bajar barrera
	  softPwmWrite(GPIO_BARRIER_PWM, NINETY_DEGREE);
	  sleep(1);
	  softPwmWrite(GPIO_BARRIER_PWM, STOP_PWM);
	}
	else {
	  // Subir barrera
	  softPwmWrite(GPIO_BARRIER_PWM, ZERO_DEGREE);
	  sleep(1);
	  softPwmWrite(GPIO_BARRIER_PWM, STOP_PWM);
	  digitalWrite(GPIO_SBARRIER, LOW);
	  digitalWrite(GPIO_SEMAFORO, LOW);
	}
      }
    }

    // 'Steam train' state
    else if (s2_state=='S'){
      if (digitalRead(GPIO_IR2_BAJO) == LOW){       // Bottom sensor. LOW = No train
	s_count++;            
	if (s_count == IDLE_COUNT) {
	  s_count = 0;
	  s2_state = 'I';
	  steam_c.prev_sector = steam_c.current_sector;
	  if (steam_c.sentido == FORWARD) steam_c.current_sector = '2';
	  else steam_c.current_sector = '1';

	  //CalculoTiempoRestante_sector (&steam_c);

	  steam_c.prev_time = steam_c.current_time;
	  steam_c.current_time = rt_timer_ticks2ns(s2_ticks_temp);
	}
      }
      else
	s_count = 0;
    }
    
    // 'Checking' state
    else if (s2_state=='C'){
        
      if (digitalRead(GPIO_IR2_ALTO) == HIGH){    // Top sensor. HIGH = Steam train
	// Change to the 'Steam train' state and save time information
	s2_state = 'S';
	s2_ticks_temp = rt_timer_read();
      }
        
      else if (digitalRead(GPIO_IR2_BAJO) == LOW){// Bottom sensor. LOW = Diesel train
	d_count++;            
	if (d_count == IDLE_COUNT){
	  d_count = 0;
	  s2_state = 'I';
	  diesel_c.prev_sector = diesel_c.current_sector;
	  if (diesel_c.sentido == FORWARD)	diesel_c.current_sector = '2';			
	  else diesel_c.current_sector = '1';

	  //CalculoTiempoRestante_sector (&diesel_c);

	  diesel_c.prev_time = diesel_c.current_time;
	  diesel_c.current_time = rt_timer_ticks2ns(s2_ticks_temp);
	}
      } else {
	d_count = 0;
      }
    }
    // END OF SECTOR 2 ---------------------------------------------------------------------------------------------------  

    // SECTOR 3 ----------------------------------------------------------------------------------------------------------
    // 'Idle' state
    if (s3_state=='I'){
      if (digitalRead(GPIO_IR3_ALTO) == HIGH){   // Top sensor. HIGH = Steam train
	// Change to the 'Steam train' state and save the time information
	s3_state = 'S';
	s3_ticks_temp = rt_timer_read();
	if (steam_c.sentido == FORWARD) {
	  // Subir barrera
	  softPwmWrite(GPIO_BARRIER_PWM, ZERO_DEGREE);
	  sleep(1);
	  softPwmWrite(GPIO_BARRIER_PWM, STOP_PWM);
	  digitalWrite(GPIO_SBARRIER, LOW);
	  digitalWrite(GPIO_SEMAFORO, LOW);
	}
	else {
	  // Bajar barrera
	  softPwmWrite(GPIO_BARRIER_PWM, NINETY_DEGREE);
	  sleep(1);
	  softPwmWrite(GPIO_BARRIER_PWM, STOP_PWM);
	}
      }
      else if (digitalRead(GPIO_IR3_BAJO) == HIGH){// Bottom sensor. HIGH = Diesel train or the first part of the Steam train.
	// Change to the 'Checking' state and save the time information
	s3_state = 'C';
	s3_ticks_temp = rt_timer_read();
	if (diesel_c.sentido == FORWARD) {
	  // Subir barrera
	  softPwmWrite(GPIO_BARRIER_PWM, ZERO_DEGREE);
	  sleep(1);
	  softPwmWrite(GPIO_BARRIER_PWM, STOP_PWM);
	  digitalWrite(GPIO_SBARRIER, LOW);
	  digitalWrite(GPIO_SEMAFORO, LOW);
	}
	else {
	  // Bajar barrera
	  softPwmWrite(GPIO_BARRIER_PWM, NINETY_DEGREE);
	  sleep(1);
	  softPwmWrite(GPIO_BARRIER_PWM, STOP_PWM);
	}
      }
    }

    // 'Steam train' state
    else if (s3_state=='S'){
      if (digitalRead(GPIO_IR3_BAJO) == LOW){       // Bottom sensor. LOW = No train
	s_count++;            
	if (s_count == IDLE_COUNT) {
	  s_count = 0;
	  s3_state = 'I';
	  steam_c.prev_sector = steam_c.current_sector;
	  if (steam_c.sentido == FORWARD) steam_c.current_sector = '3';
	  else {
	    steam_c.current_sector = '2';
	    //			softPwmWrite(GPIO_BARRIER_PWM, NINETY_DEGREE);
	  }

	  //CalculoTiempoRestante_sector (&steam_c);

	  steam_c.prev_time = steam_c.current_time;
	  steam_c.current_time = rt_timer_ticks2ns(s3_ticks_temp);   
	  //		if (steam_c.sentido == FORWARD || diesel_c.sentido == FORWARD) softPwmWrite(GPIO_BARRIER_PWM, ZERO_DEGREE);
	}
      }
      else
	s_count = 0;
    }
    
    // 'Checking' state
    else if (s3_state=='C'){
        
      if (digitalRead(GPIO_IR3_ALTO) == HIGH){    // Top sensor. HIGH = Steam train
	// Change to the 'Steam train' state and save time information
	s3_state = 'S';
	s3_ticks_temp = rt_timer_read();
      }
        
      else if (digitalRead(GPIO_IR3_BAJO) == LOW){// Bottom sensor. LOW = Diesel train
	d_count++;            
	if (d_count == IDLE_COUNT){
	  d_count = 0;
	  s3_state = 'I';
	  diesel_c.prev_sector = diesel_c.current_sector;
	  if (diesel_c.sentido == FORWARD) diesel_c.current_sector = '3';
	  else {
	    diesel_c.current_sector = '2';
	    //			softPwmWrite(GPIO_BARRIER_PWM, NINETY_DEGREE);
	  }

	  //CalculoTiempoRestante_sector (&diesel_c);

	  diesel_c.prev_time = diesel_c.current_time;
	  diesel_c.current_time = rt_timer_ticks2ns(s3_ticks_temp);
	  //		if (steam_c.sentido == FORWARD || diesel_c.sentido == FORWARD) softPwmWrite(GPIO_BARRIER_PWM, ZERO_DEGREE);
	}
      } else {
	d_count = 0;
      }
    }
    // END OF SECTOR 3 ---------------------------------------------------------------------------------------------------    
    
    // Release mutex_steam and mutex_diesel
    //rt_mutex_release(&mutex_steam);

    rt_mutex_acquire(&mutex_diesel,TM_INFINITE);
    diesel_t.prev_sector = diesel_c.prev_sector; diesel_t.prev_time = diesel_c.prev_time;
    diesel_t.current_sector = diesel_c.current_sector; diesel_t.current_time = diesel_c.current_time;
    steam_t.prev_sector = steam_c.prev_sector; steam_t.prev_time = steam_c.prev_time;
    steam_t.current_sector = steam_c.current_sector; steam_t.current_time = steam_c.current_time;
    rt_mutex_release(&mutex_diesel);

  }
}


// sector: el sector por el que acaba de pasar el tren
double calcularVelocidad(struct train *tren) {
	double longitud;
	double velCmS;
	if ((*tren).velocidad == 0) return 0;


	
	if(((*tren).current_sector == '0' || (*tren).current_sector == '2')&& pVia == '1') {
		longitud = 48*3.1416;
	} else if(((*tren).current_sector == '0' || (*tren).current_sector == '2') && pVia == '0'){
		longitud = 45*3.1416;
	}else {
		longitud = 90;
	}
	
	
	if (((*tren).current_time == 0) || ((*tren).prev_time == 0))
		velCmS = 0;
	else{
		SRTIME intervalo = (*tren).current_time - (*tren).prev_time;
		double intervDouble = (double) intervalo;
		velCmS = longitud/(intervDouble/1000000000);
	}
	(*tren).velocidad_fis = velCmS;
	return velCmS;
}





void imprimirInterfazInicial() {
	/* ----------------- COLORS SET -------------------
		COLOR_BLACK   0
        	COLOR_RED     1
        	COLOR_GREEN   2
        	COLOR_YELLOW  3
        	COLOR_BLUE    4
        	COLOR_MAGENTA 5
        	COLOR_CYAN    6
        	COLOR_WHITE   7				*/

	initscr();			/* Start curses mode 		*/
		if(has_colors() == FALSE)
		{	endwin();
			mvprintw(13, 3, "Your Terminal does not support color WTF");
			exit(1);
		}

	start_color();			/* Start color 			*/
	init_pair(1, COLOR_BLACK, COLOR_WHITE);	
	init_pair(2, COLOR_CYAN, COLOR_BLACK);
	init_pair(3, COLOR_GREEN, COLOR_BLACK);
	
	attron(COLOR_PAIR(1));
	//mvprintw(2, 36, "CONTROL DE TRENES");
	mvprintw(2, 28, "CONTROL DE TRENES");
	attroff(COLOR_PAIR(1));
	
	attron(COLOR_PAIR(2));
	mvprintw(4, 3, "Tren");
	mvprintw(4, 18, "Sector");
	mvprintw(4, 29, "Velocidad (cm/s)");
	mvprintw(4, 52, "Sentido");
	mvprintw(4, 66, "Potencia");
	mvprintw(5, 3, "DIESEL");
	mvprintw(5, 18, "0");
	mvprintw(5, 33, "0");

	mvprintw(6, 3, "VAPOR");
	mvprintw(6, 18, "0");
	mvprintw(6, 33, "0");

	mvprintw(8, 3, "Cambio via");	// Ojo: por alguna razón meter vocales acentuadas descuadra las coordenadas
	mvprintw(8, 18, "Estado");
	mvprintw((6-trenActual), 18, "*");	
	//mvprintw(11, 3, "(0-9) Ajustar vel. tren selec. - (C) Cambio de vía - (T) Cambiar tren selec.");	
	mvprintw(11, 3, "(C) Cambio de via - (0) Parar tren - (+/-) Aumentar/Disminuir potencia");
	mvprintw(12, 3, "(t) Seleccion de tren - (r) Activar Sistema de Rutas - (e) Exit/Salida");
	
	int i;
	
	for (i = TIMER_CUAD_LEFT; i< TIMER_CUAD_RIGHT; i++){
		//mvprintw(TIMER_CUAD_UP-1, i+1, "%c", c);
		//mvprintw(TIMER_CUAD_DOWN-1,i, "%c", c);
		move(TIMER_CUAD_UP-1,i+1); 		
		addch(ACS_HLINE);
		move(TIMER_CUAD_DOWN-1,i);
		addch(ACS_HLINE);
	}
	mvprintw(TIMER_CUAD_UP-1,i, " ");
	
	
	for (i = TIMER_CUAD_UP; i< TIMER_CUAD_DOWN-1; i++){
		//mvprintw(i, TIMER_CUAD_RIGHT, "|");
		//mvprintw(i, TIMER_CUAD_LEFT, "|");
		move(i,TIMER_CUAD_RIGHT); 		
		addch(ACS_VLINE);
		move(i,TIMER_CUAD_LEFT);
		addch(ACS_VLINE);
	}
	
	//dibujamos las equinas
	move(TIMER_CUAD_UP-1,TIMER_CUAD_RIGHT);
	addch(ACS_URCORNER);	
	move(TIMER_CUAD_UP-1,TIMER_CUAD_LEFT);
	addch(ACS_ULCORNER);
	move(TIMER_CUAD_DOWN-1,TIMER_CUAD_RIGHT);
	addch(ACS_LRCORNER);
	move(TIMER_CUAD_DOWN-1,TIMER_CUAD_LEFT);
	addch(ACS_LLCORNER);

	mvprintw(TIMER_CUAD_UP+1,TIMER_CUAD_LEFT+2, "Siguiente tren en:");
	mvprintw(TIMER_CUAD_UP+3,TIMER_CUAD_LEFT+6, "Diesel");
	mvprintw(TIMER_CUAD_UP+3,TIMER_CUAD_LEFT+6+TIMER_CUAD_SPLIT, "Vapor");


	attroff(COLOR_PAIR(2));
	attroff(COLOR_PAIR(7));
	mvprintw(5, 52, "Pa'rao  ");
	mvprintw(6, 52, "Pa'rao  ");
	attroff(COLOR_PAIR(7));
	
}


void Apantalla(void *arg)
{
  double VelDiesel, VelVapor;
  rt_task_set_periodic(NULL, TM_NOW, 100000000);


  while(1){
    struct train diesel_c, steam_c;
    mvprintw(1,1,(time(NULL)%2)?"*":"-");

    // Blocks the while loop till the next periodic interruption
    rt_task_wait_period(NULL);

    // Acquire mutex_steam and mutex_diesel
    //rt_mutex_acquire(&mutex_steam,TM_INFINITE);
    rt_mutex_acquire(&mutex_diesel,TM_INFINITE);
    diesel_c = diesel_t;
    steam_c = steam_t;
    rt_mutex_release(&mutex_diesel);

    mvaddch(5, 18, diesel_c.current_sector);
    mvaddch(6, 18, steam_c.current_sector);
		
    if (pVia == '0')
      mvprintw(9, 18, "Via interior");
    else{
      mvprintw(9, 18, "Via exterior");
    }

    VelDiesel=calcularVelocidad(&diesel_c);
    VelVapor=calcularVelocidad(&steam_c);
    CalculoTiempoRestante_sector (&diesel_c);
    CalculoTiempoRestante_sector (&steam_c);

    if (VelDiesel == 0)
      mvprintw(5, 29, "No Data");			
    else
      mvprintw(5, 29, "%0.2f   ", VelDiesel);

    if (VelVapor == 0)
      mvprintw(6, 29, "No Data");			
    else
      mvprintw(6, 29, "%0.2f   ", VelVapor);

    if (trenActual == 1){
      mvprintw(5, 2, "*");
      mvprintw(6, 2, " ");
    }else{
      mvprintw(6, 2, "*");
      mvprintw(5, 2, " ");
    }
		

    if(rutasActiva == '1')
      {
	//mvprintw(26, 3, "Sistema de rutas activo, proximo destino: t:%c s:%c v:%d  d:%c v:%d", terminatmp, sectortmp , velocidadtmp, direcciontmp, cambioviatmp);
	mvprintw(26, 3, "Sistema de rutas activo  ");
      }
    else if ( rutasActiva == '0')
      { 
		
	mvprintw(26, 3, "Sistema de rutas inactivo");
      }	
    init_pair(4, COLOR_YELLOW, COLOR_BLACK);

    attron(COLOR_PAIR(4));
    mvprintw(15, 3, ">>");
    int i = 0;
    while(in[i] != '\0')
      i++;
    move(15,(5+i));			
    attroff(COLOR_PAIR(4));

		

    /* ----------------- COLORS SET -------------------
       COLOR_BLACK   0
       COLOR_RED     1
       COLOR_GREEN   2
       COLOR_YELLOW  3
       COLOR_BLUE    4
       COLOR_MAGENTA 5
       COLOR_CYAN    6
       COLOR_WHITE   7				*/

    //start_color();			/* Start color 			*/
    //init_pair(1, COLOR_RED, COLOR_BLACK);

	
    refresh();

    // Release mutex_steam and mutex_diesel
    //rt_mutex_release(&mutex_steam);
    //rt_mutex_release(&mutex_diesel);
    // rt_mutex_release(&mutex_pVia);
    rt_mutex_acquire(&mutex_diesel,TM_INFINITE);
    diesel_t.velocidad_fis = diesel_t.velocidad_fis;
    steam_t.velocidad_fis = steam_t.velocidad_fis;
    diesel_t.tiempo_restante = diesel_t.tiempo_restante;
    steam_t.tiempo_restante = steam_t.tiempo_restante;
    diesel_t.dist_restante = diesel_t.dist_restante;
    steam_t.dist_restante = steam_t.dist_restante;
    rt_mutex_release(&mutex_diesel);
  }

}


void cambiarTren(void)
{
	if(trenActual == 0) trenActual = 1;
	else trenActual = 0;
}
// variableInit Function
int variableInit (void)
{
	// Inicialización estructuras train.
	steam_t.train_type = TREN_CARBON;
	steam_t.current_sector = '0';
	steam_t.current_time = 0;
	steam_t.prev_sector = '0';
	steam_t.prev_time = 0;
	diesel_t.train_type = TREN_GASOIL;
	diesel_t.current_sector = '0';
	diesel_t.current_time = 0;
	diesel_t.prev_sector = '0';
	diesel_t.prev_time = 0;
	pVia = '1';
	s0_state = 'I';
    	s1_state = 'I';
    	s2_state = 'I';
    	s3_state = 'I';
	s_count = 0;
	d_count = 0;
	steam_t.dist_restante = VUELTA_COMPLETA;
	diesel_t.dist_restante = VUELTA_COMPLETA;
	pinMode (GPIO_SBARRIER, OUTPUT);
	pinMode (GPIO_SEMAFORO, OUTPUT);
	digitalWrite(GPIO_SBARRIER, LOW);
	digitalWrite(GPIO_SEMAFORO, LOW);
	softPwmCreate(GPIO_BARRIER_PWM, ZERO_DEGREE, 200);
	sleep(1);
	softPwmWrite(GPIO_BARRIER_PWM, STOP_PWM);
	return 0;
}

void catch_signal() {}



/*Still testing : 20/04 */
int interfaz_usuario(void)
{	

  while(1)
    {
      
      mvprintw(15, 3, ">>");
      clrtoeol();
      in[0] = '\0';
      getstr(in);

      if(in[0] == 'r')
	{
	  iniciaRuta();
	  rutasActiva = '1';	
	}
		
      if((in[0] == '+')||(in[0] == '-')){
	int i = 0;
	int j = 0;
	while((in[i] == '+')||(in[i] == '-')){
	  if(in[i] == '+'){
	    j++;
	  }
	  else{
	    j--;
	  }
	  i++;		
	}
			
	velocidad += 4*j;
			
	if(velocidad < -28){
	  velocidad = -28;
	  if ( trenActual == 1) {
	    diesel_t.velocidad = velocidad;					
	    diesel_t.sentido = REVERSE;
	  }
	  else {
	    steam_t.velocidad = velocidad;
	    steam_t.sentido = REVERSE;
	  }
	  mvprintw((6-trenActual), 52, "Pa'tras  ");
	  mvprintw((6-trenActual), 66, "    ");
	  mvprintw((6-trenActual), 66, "%d", velocidad);
	  setVelocidad((char)trenActual, REVERSE, (char) (-velocidad));
	}
	else if (velocidad < 0){
	  if ( trenActual == 1) {
	    diesel_t.velocidad = velocidad;
	    diesel_t.sentido = REVERSE;
	  }
	  else {
	    steam_t.velocidad = velocidad;
	    steam_t.sentido = REVERSE;
	  }
				
	  mvprintw((6-trenActual), 52, "Pa'tras  ");
	  mvprintw((6-trenActual), 66, "    ");
	  mvprintw((6-trenActual), 66, "%d", velocidad);
	  setVelocidad((char)trenActual, REVERSE, (char) (-velocidad));
	}
	else if (velocidad == 0){
	  if ( trenActual == 1) {
	    diesel_t.velocidad = velocidad;
	    diesel_t.sentido = FORWARD;
	  }
	  else {
	    steam_t.velocidad = velocidad;
	    steam_t.sentido = FORWARD;
	  }

	  mvprintw((6-trenActual), 52, "Pa'rao   ");
	  mvprintw((6-trenActual), 66, "    ");
	  mvprintw((6-trenActual), 66, "%d", velocidad);
	  setVelocidad((char)trenActual, FORWARD, (char) velocidad);
	}
	else if (velocidad <= 28){
	  if ( trenActual == 1) {
	    diesel_t.velocidad = velocidad;
	    diesel_t.sentido = FORWARD;
	  }
	  else {
	    steam_t.velocidad = velocidad;
	    steam_t.sentido = FORWARD;
	  }
				
	  mvprintw((6-trenActual), 52, "Pa'lante");
	  mvprintw((6-trenActual), 66, "    ");
	  mvprintw((6-trenActual), 66, "%d", velocidad);
	  setVelocidad((char)trenActual, FORWARD, (char) velocidad);
	}
	else {
	  velocidad = 28;
	  if ( trenActual == 1) {
	    diesel_t.velocidad = velocidad;
	    diesel_t.sentido = FORWARD;
	  }
	  else {
	    steam_t.velocidad = velocidad;
	    steam_t.sentido = FORWARD;
	  }				
	  mvprintw((6-trenActual), 52, "Pa'lante");			
	  mvprintw((6-trenActual), 66, "    ");
	  mvprintw((6-trenActual), 66, "%d", velocidad);
	  setVelocidad((char)trenActual, FORWARD, (char) velocidad);
	}
	/*if ( trenActual == 1) {
	  diesel_t.velspot_time_prev =  diesel_t.velspot_time_current;					
	  diesel_t.velspot_time_current = rt_timer_ticks2ns(rt_timer_read());
	  }
	  else {
	  steam_t.velspot_time_prev =  steam_t.velspot_time_current;					
	  steam_t.velspot_time_current = rt_timer_ticks2ns(rt_timer_read());
	  }*/

					
			
      }	
      else if (in[0] == '0')
	{	
	  velocidad = 0;

	  if ( trenActual == 1) {
	    diesel_t.velocidad = 0;
	    //CalculoTiempoRestante_sector (&diesel_t);
	  }
	  else{
	    steam_t.velocidad = 0;
	    //CalculoTiempoRestante_sector (&steam_t);
	  }

	  mvprintw((6-trenActual), 52, "Pa'rao   ");
	  mvprintw((6-trenActual), 66, "    ");
	  mvprintw((6-trenActual), 66, "%d", velocidad);
	  setVelocidad((char)trenActual, FORWARD, velocidad);	
		
				
	}	
      else if(in[0]  == 'c') {
	cambiarVia(2);
	// Si pVia = 0 seleciona la via interior
      }else if(in[0]  == 't') {
	cambiarTren();
      }else if(in[0]  == 'e') {
	// No se puede acabar el programa con Ctl+C o Ctl+Z porque ncurses se vuelve loco y hay que rebootear la RPi
	// Hay que terminar siempre el programa con el comando "e"
	paraTren(); //paramos el tren para evitar el offset del ampli			
	endwin();
	rt_task_delete(&Actualiza_pantalla);
	rt_task_delete(&IRtracking_task);
	rt_task_delete(&controldeVelocidad);
	exit(0);
			
      }
    }
  system("reset");	// Reset del terminal
  return 0;
}



// Main
int main(int argc, char* argv[])
{

	// Blocks interruptions and stuff
	signal(SIGTERM, catch_signal);
	signal(SIGINT, catch_signal);
	/* Create a mutex; we could also have attempted to bind to some
	pre-existing object, using rt_mutex_bind() and rt_mutex_bind()
	instead of creating it. In any case, priority inheritance is
	automatically enforced for mutual exclusion locks. */

	err_steam = rt_mutex_create(&mutex_steam,"SteamMutex");
	err_diesel = rt_mutex_create(&mutex_diesel,"DieselMutex");
	err_pVia = rt_mutex_create(&mutex_pVia,"pViaMutex");
	


	// Función de configuración utilizada en el tutorial (?comprobar su utilidad):
	if (wiringPiSetup () == -1)
		exit (1) ;

	// Inicialización de todas las variables.
	if (variableInit () == -1)
		exit (1) ;

	// Se configuran los pines 0 al 7 como entrada -> Sensores IR
	for (pin = 0 ; pin < 8 ; pin++)
		pinMode (pin, INPUT);

	if(initIO() == -1)
		exit(1);
	// Se configura el pin 10 como salida -> cambio de via
	pinMode (GPIO_CAMBIO_VIA,OUTPUT);

       	digitalWrite (GPIO_CAMBIO_VIA,1) ; // Empieza por la via exterior


	char str[10] ;
	// Perform auto-init of rt_print buffers if the task doesn't do so
	rt_print_auto_init(1);

	// Lock memory : avoid memory swapping for this program
	mlockall(MCL_CURRENT|MCL_FUTURE);

	// rt_printf("start task\n");

	/*
	* Arguments: &task,
	* name,
	* stack size (0=default),
	* priority,
	* mode (FPU, start suspended, ...)
	*/

        rt_task_create(&IRtracking_task, str, 0, 50, 0);
	rt_task_create(&Actualiza_pantalla, str, 0, 40, 0);
	rt_task_create(&controldeVelocidad, str, 0, 52,0);
	rt_task_create(&ruta, str, 0, 51,0);
	initscr();			/* Start curses mode */
	//noecho();
	curs_set(0);
	imprimirInterfazInicial();

	rt_task_start(&IRtracking_task, &IRtracking, 0);
	rt_task_start(&Actualiza_pantalla, &Apantalla, 0);
	rt_task_start(&controldeVelocidad, &controlVelocidad, 0);
	rt_task_start(&ruta, &miraPosicion, 0);
	interfaz_usuario();	
	rt_task_delete(&Actualiza_pantalla);
	rt_task_delete(&IRtracking_task);
	rt_task_delete(&controldeVelocidad);
	rt_task_delete(&ruta);
	endwin();			/* End curses mode		  */
	// cleanup();
	

	return 0;

}

// Borrado del MUTEX:
void cleanup (void)
{
	rt_mutex_delete(&mutex_steam);
	rt_mutex_delete(&mutex_diesel);
}
