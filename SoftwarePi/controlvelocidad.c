#include <stdio.h>
#include <wiringPi.h>
#include <native/task.h>
#include <rtdk.h>
#include <stdint.h>
#include <ncurses.h>
#include "controlvelocidad.h"
#include "trama.h"


static TRM trama[N_TRENES];
unsigned long long	 bufferSetVel = 0ULL;
char tramaPendiente = 0;


/*
void controlVias(){
     unsigned long long buffer = 0;
     const unsigned long long tramaIdle = 0x3FE007FBFFF;  
     char bufferBit = 0xFF;

     rt_task_set_periodic(NULL, TM_NOW, 58000000);

     while(1){
              rt_task_wait_period(NULL);
              
              if (bufferBit != 0xFF){ // buffer de salida no está vacío
	             digitalWrite (9, bufferBit & 0x01);
                 bufferBit = bufferBit >> 1;
                 bufferBit = bufferBit | 0x80;
              }else{
                    if (buffer != 0) { // buffer de trama no está vacio
                       if ((buffer & 0x1) == 1){// conversión de bits lógicos a bits para la modulación
                          bufferBit = 0xFD;
                       }else{
                          bufferBit = 0xF3;
                       }
		               buffer = buffer >> 1;
		               digitalWrite (9, bufferBit & 0x01);
		               bufferBit = bufferBit >> 1;
		               bufferBit = bufferBit | 0x80;
	                }else{
                          if (tramaPendiente == 1){
			                 buffer = bufferSetVel;
                             tramaPendiente = 0;
                          }else{
			                    buffer = tramaIdle;
                          }
                          if ((buffer & 0x1) == 1){// conversión de bits lógicos a bits para la modulación
		                     bufferBit = 0xFD;
                          }else{ 
		                  bufferBit = 0xF3;
                          }
	                      buffer = buffer >> 1;
	                      digitalWrite (9, bufferBit & 0x01);
	                      bufferBit = bufferBit >> 1;
	                      bufferBit = bufferBit | 0x80;
                    }
              }
}

/** FUNCION ALTERNATIVA (ECHAR UN VISTAZO PLZ) **/

void controlVelocidad(){
     unsigned long long buffer = 0;
     unsigned long long tramaIdle = 0xFFFDFE007FC00000ULL;      
     //unsigned long long tramaIdle = 0x6B3D840FFFULL; 
     char bufferBit = 0xFF;
     rt_task_set_periodic(NULL, TM_NOW, 58000);
     while(1){
              rt_task_wait_period(NULL);
              if (bufferBit == 0xFF){
                 if(buffer == 0){
                           if (tramaPendiente == 1){
			                  buffer = bufferSetVel;
					  tramaIdle = buffer;					 
				          //mvprintw(16, 3,"%llu",buffer);
                              tramaPendiente = 0;
                           }else{
                              buffer = tramaIdle;

                           }
                 }
                 if ((buffer & 0x8000000000000000ULL) == 0x8000000000000000ULL){
                    bufferBit = 0xFD;
                 }else{
                    bufferBit = 0xF3;
                 }
	       buffer = buffer << 1;
	       
              }
              digitalWrite (LINE_OUT, (bufferBit & 0x01));
	      bufferBit = bufferBit >> 1;
              bufferBit = bufferBit | 0x80;
     }                     
}    
               


int initIO(){
	int k = 0;	
	for(k = 0; k < 2; k++){
		trama[k].address.field_add.un1 = 0;
		if (k == TREN_CARBON){
			trama[k].address.field_add.add = TREN_CARBON_ADDRESS;
		}
		else{
			trama[k].address.field_add.add = TREN_GASOIL_ADDRESS;
		}
		trama[k].control.field_ctr.un2 = 0b01;
		trama[k].control.field_ctr.dir = FORWARD;
		trama[k].control.field_ctr.vel = getVelocidadCode(0);
		trama[k].error = trama[k].address.all_add ^ trama[k].control.all_ctr;
	}	
	
		pinMode (LINE_OUT, OUTPUT);

		return 0;
}


void putTrama(char tren){
     unsigned long long bufferTemp;
     //Base de trama
     bufferTemp = 0xFFFC004000400000ULL;
     bufferTemp = bufferTemp | ((unsigned long long) trama[tren].address.all_add << 41);
     bufferTemp = bufferTemp | ((unsigned long long) trama[tren].control.all_ctr << 32);
     bufferTemp = bufferTemp | ((unsigned long long) (trama[tren].address.all_add ^ trama[tren].control.all_ctr) << 23);
     bufferSetVel = bufferTemp;
     char tramaPendiente = 1;
}

	

void setVelocidad(char tren, char direccion, char velocidad){
	trama[tren].address.field_add.un1 = 0;
	if (tren == TREN_CARBON){
		trama[tren].address.field_add.add = TREN_CARBON_ADDRESS;
	}
	else{
		trama[tren].address.field_add.add = TREN_GASOIL_ADDRESS;
	}
	trama[tren].control.field_ctr.un2 = 0b01;
	trama[tren].control.field_ctr.dir = direccion;
	trama[tren].control.field_ctr.vel = getVelocidadCode(velocidad);
	trama[tren].error = trama[tren].address.all_add ^ trama[tren].control.all_ctr;
	
	//TrenPendiente = tren;
	tramaPendiente = 1;
	//while(TramaPendiente == 1);	
	putTrama(tren);
}

char getVelocidadCode(char velocidad){
	char resultado;
	switch (velocidad) {
		case 1:
			resultado=STEP_1;
			break;
		case 2:
			resultado=STEP_2;
                        break;
		case 3:
			resultado=STEP_3;
                        break;
		case 4:
			resultado=STEP_4;
                        break;
		case 5:
			resultado=STEP_5;
                        break;
		case 6:
			resultado=STEP_6;
                        break;
		case 7:
			resultado=STEP_7;
                        break;
		case 8:
			resultado=STEP_8;
                        break;
		case 9:
			resultado=STEP_9;
                        break;
		case 10:
			resultado=STEP_10;
                        break;
		case 11:
			resultado=STEP_11;
                        break;
		case 12:
			resultado=STEP_12;
                        break;
		case 13:
			resultado=STEP_13;
                        break;
		case 14:
			resultado=STEP_14;
                        break;
		case 15:
			resultado=STEP_15;
                        break;
		case 16:
                        resultado=STEP_16;
                        break;
		case 17:
                        resultado=STEP_17;
                        break;
		case 18:
                        resultado=STEP_18;
                        break;
		case 19:
                        resultado=STEP_19;
                        break;
		case 20:
                        resultado=STEP_20;
                        break;
		case 21:
                        resultado=STEP_21;
                        break;
		case 22:
                        resultado=STEP_22;
                        break;
		case 23:
                        resultado=STEP_23;
                        break;
		case 24:
                        resultado=STEP_24;
                        break;
		case 25:
                        resultado=STEP_25;
                        break;
		case 26:
                        resultado=STEP_26;
                        break;
		case 27:
                        resultado=STEP_27;
                        break;
		case 28:
                        resultado=STEP_28;
                        break;
		case 0:
                        resultado=STOP;
                        break;
		default:
			resultado=STOP;
			break;
		}
	return resultado;
}

/****************************************************************************/

/*
void putTramaIdle(){
	//printf("Trama Idle==>");
	//Enviamos cabecera (12 unos)	
	int i;	
	for (i = 0; i < CABECERA_LENGHT; i++) {
		putUno();
	}

	//Separacion
	//printf(" ");
	putCero();
	//printf(" ");

	//Enviamos primer byte Idle
	putChar(0xFF);

	//Separacion
	//printf(" ");
	putCero();
	//printf(" ");

	//Enviar velocidad y sentido
	putChar(0x00);

	//Separacion
	//printf(" ");
	putCero();
	//printf(" ");

	//Corrección de errores
	putChar(0xFF);
	
	//Bit de terminacion
	//printf(" ");
	putUno();
	//printf(" \r\n");	
}
*/	

/*
void putUno(){
	digitalWrite (LINE_OUT, 1);	// On
    	delayMicroseconds(DELAY);
	//usleep(DELAY);	// Delay 58 mS
    	digitalWrite (LINE_OUT, 0);	// Off
    	delayMicroseconds(DELAY);
	//usleep(DELAY);	// Delay 58 mS
	//printf("1");
}

void putCero(){
	digitalWrite (LINE_OUT, 1);	// On 
    	delayMicroseconds(DELAY);
	delayMicroseconds(DELAY);
	//usleep(2*DELAY);	// Delay 116 mS
    	digitalWrite (LINE_OUT, 0);	// Off
    	delayMicroseconds(DELAY);
	delayMicroseconds(DELAY);
	//usleep(2*DELAY);	// Delay 116 mS
	//printf("0");
}
*/

/*
void putChar(char c){
	int i;
	for (i = 0; i < 8; i++){
		if((c >> (7-i) & 0x01) == 0x01){
			putUno();
		}
		else{
			putCero();
		}
	}
}
*/

//unsigned char TrenPendiente = 0;

/*
int main (void){
  printf ("Raspberry Pi blink\n") ;

  if (!initIO()){
	//for (;;){
		//putUno();
		//putCero();
		//putChar('A');
		//trama[0].address.field_add.add = 0x7A;	
		//trama[0].control.field_ctr.vel = STOP;
		//trama[0].control.field_ctr.dir = FORWARD;
		//setVelocidad(TREN_CARBON, FORWARD, 1);
		//printf("Caracter devuelto (1) ==> %d", getVelocidadCode(1));
	//}
	return 0;	
  }
  else{
	return 1;
  }
}
*/

/*
PI_THREAD (controlVias){
	while(1){
		while(TramaPendiente == 0){
			putTramaIdle();			
			}
		putTrama(TrenPendiente);
		TramaPendiente = 0;
	}
}
*/
