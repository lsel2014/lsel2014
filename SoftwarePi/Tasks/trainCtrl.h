
#ifndef TRAINCTRL_H
#define TRAINCTRL_H

#define	LINE_OUT	12

#define TASK_TRAINCTRL_PERIOD 58000

#define DELAY		58
#define CABECERA_LENGHT	12	
#define	N_TRENES	2

#define STOP		0b00000
#define STOP_I		0b10000
#define E_STOP_I	0b00001
#define STEP_1		0b00010
#define STEP_2		0b10010
#define STEP_3		0b00011
#define STEP_4		0b10011
#define STEP_5		0b00100
#define STEP_6		0b10100
#define STEP_7		0b00101
#define STEP_8		0b10101
#define STEP_9		0b00110
#define STEP_10		0b10110
#define STEP_11		0b00111
#define STEP_12		0b10111
#define STEP_13		0b01000
#define STEP_14		0b11000
#define STEP_15		0b01001
#define STEP_16		0b11001
#define STEP_17		0b01010
#define STEP_18		0b11010
#define STEP_19		0b01011
#define STEP_20		0b11011
#define STEP_21		0b01100
#define STEP_22		0b11100
#define STEP_23		0b01101
#define STEP_24		0b11101
#define STEP_25		0b01110
#define STEP_26		0b11110
#define STEP_27		0b01111
#define STEP_28		0b11111

#define FORWARD   1
#define REVERSE   0

#define TREN_CARBON  0
#define TREN_GASOIL  1

#define TREN_CARBON_ADDRESS 0b0000011
#define TREN_GASOIL_ADDRESS 0b0000100

#define VELFIS_28	1
#define VELFIS_24	1
#define VELFIS_20	1
#define VELFIS_16	1
#define VELFIS_12	1
#define VELFIS_8	1
#define VELFIS_4	1
#define VELFIS_0	1
#define VELFIS_28n	1
#define VELFIS_24n	1
#define VELFIS_20n	1
#define VELFIS_16n	1
#define VELFIS_12n	1
#define VELFIS_8n	1
#define VELFIS_4n	1

int trainCtrl_init();

void trainCtrl_putTrama(char tren);

void trainCtrl_setVelocidad(char tren, char direccion, char velocidad);

void trainCtrl_periodic();

char trainCtrl_getVelocidadCode(char velocidad);

#endif
