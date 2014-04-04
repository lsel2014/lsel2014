#include <stdio.h>
#include <wiringPi.h>
#include <native/task.h>
#include <rtdk.h>
#include <stdint.h>
#include <ncurses.h>

#include "trainCtrl.h"
#include "trainCtrlTrama.h"

static TRM trama[N_TRENES];
unsigned long long bufferSetVel = 0ULL;
char tramaPendiente = 0;

void trainCtrl_periodic() {
	unsigned long long buffer = 0;
	unsigned long long tramaIdle = 0xFFFDFE007FC00000ULL;

	char bufferBit = 0xFF;

	rt_task_set_periodic(NULL, TM_NOW, 58000);

	while (1) {
		rt_task_wait_period(NULL );
		if (bufferBit == 0xFF) {
			if (buffer == 0) {
				if (tramaPendiente == 1) {
					buffer = bufferSetVel;
					tramaIdle = buffer;
					//mvprintw(16, 3,"%llu",buffer);
					tramaPendiente = 0;
				} else {
					buffer = tramaIdle;

				}
			}
			if ((buffer & 0x8000000000000000ULL) == 0x8000000000000000ULL) {
				bufferBit = 0xFD;
			} else {
				bufferBit = 0xF3;
			}
			buffer = buffer << 1;

		}
		digitalWrite(LINE_OUT, (bufferBit & 0x01));
		bufferBit = bufferBit >> 1;
		bufferBit = bufferBit | 0x80;
	}
}

int trainCtrl_init() {
	int k = 0;
	for (k = 0; k < 2; k++) {
		trama[k].address.field_add.un1 = 0;
		if (k == TREN_CARBON) {
			trama[k].address.field_add.add = TREN_CARBON_ADDRESS;
		} else {
			trama[k].address.field_add.add = TREN_GASOIL_ADDRESS;
		}
		trama[k].control.field_ctr.un2 = 0b01;
		trama[k].control.field_ctr.dir = FORWARD;
		trama[k].control.field_ctr.vel = getVelocidadCode(0);
		trama[k].error = trama[k].address.all_add ^ trama[k].control.all_ctr;
	}

	pinMode(LINE_OUT, OUTPUT);

	return 0;
}

void trainCtrl_putTrama(char tren) {
	unsigned long long bufferTemp;
	//Base de trama
	bufferTemp = 0xFFFC004000400000ULL;
	bufferTemp = bufferTemp
			| ((unsigned long long) trama[tren].address.all_add << 41);
	bufferTemp = bufferTemp
			| ((unsigned long long) trama[tren].control.all_ctr << 32);
	bufferTemp = bufferTemp
			| ((unsigned long long) (trama[tren].address.all_add
					^ trama[tren].control.all_ctr) << 23);
	bufferSetVel = bufferTemp;
	char tramaPendiente = 1;
}

void trainCtrl_setVelocidad(char tren, char direccion, char velocidad) {
	trama[tren].address.field_add.un1 = 0;
	if (tren == TREN_CARBON) {
		trama[tren].address.field_add.add = TREN_CARBON_ADDRESS;
	} else {
		trama[tren].address.field_add.add = TREN_GASOIL_ADDRESS;
	}
	trama[tren].control.field_ctr.un2 = 0b01;
	trama[tren].control.field_ctr.dir = direccion;
	trama[tren].control.field_ctr.vel = trainCtrl_getVelocidadCode(velocidad);
	trama[tren].error = trama[tren].address.all_add
			^ trama[tren].control.all_ctr;

	//TrenPendiente = tren;
	tramaPendiente = 1;
	//while(TramaPendiente == 1);	
	trainCtrl_putTrama(tren);
}

char trainCtrl_getVelocidadCode(char velocidad) {
	char resultado;
	switch (velocidad) {
	case 1:
		resultado = STEP_1;
		break;
	case 2:
		resultado = STEP_2;
		break;
	case 3:
		resultado = STEP_3;
		break;
	case 4:
		resultado = STEP_4;
		break;
	case 5:
		resultado = STEP_5;
		break;
	case 6:
		resultado = STEP_6;
		break;
	case 7:
		resultado = STEP_7;
		break;
	case 8:
		resultado = STEP_8;
		break;
	case 9:
		resultado = STEP_9;
		break;
	case 10:
		resultado = STEP_10;
		break;
	case 11:
		resultado = STEP_11;
		break;
	case 12:
		resultado = STEP_12;
		break;
	case 13:
		resultado = STEP_13;
		break;
	case 14:
		resultado = STEP_14;
		break;
	case 15:
		resultado = STEP_15;
		break;
	case 16:
		resultado = STEP_16;
		break;
	case 17:
		resultado = STEP_17;
		break;
	case 18:
		resultado = STEP_18;
		break;
	case 19:
		resultado = STEP_19;
		break;
	case 20:
		resultado = STEP_20;
		break;
	case 21:
		resultado = STEP_21;
		break;
	case 22:
		resultado = STEP_22;
		break;
	case 23:
		resultado = STEP_23;
		break;
	case 24:
		resultado = STEP_24;
		break;
	case 25:
		resultado = STEP_25;
		break;
	case 26:
		resultado = STEP_26;
		break;
	case 27:
		resultado = STEP_27;
		break;
	case 28:
		resultado = STEP_28;
		break;
	case 0:
		resultado = STOP;
		break;
	default:
		resultado = STOP;
		break;
	}
	return resultado;
}

