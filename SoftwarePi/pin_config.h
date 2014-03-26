#ifndef PIN_CONFIG_H
#define PIN_CONFIG_H

#define GPIO_IR0_BAJO 0
#define GPIO_IR0_ALTO 1
#define GPIO_IR1_BAJO 2
#define GPIO_IR1_ALTO 3
#define GPIO_IR2_BAJO 4
#define GPIO_IR2_ALTO 5
#define GPIO_IR3_BAJO 6
#define GPIO_IR3_ALTO 7
#define GPIO_CAMBIO_VIA 10
#define GPIO_BARRIER_PWM 11
#define	GPIO_SBARRIER 15
#define GPIO_SEMAFORO 9

#define IRT_PERIOD 75000000 // IRtracking period: 75ms
#define IDLE_COUNT 2
#define REFRESH_PERIOD 500000000 // Periodo de refresco de pantalla

#define ZERO_DEGREE 9		// Originalmente 12
#define NINETY_DEGREE 18	// Orig. 21
#define STOP_PWM 0

#define TIMER_CUAD_UP 18
#define TIMER_CUAD_DOWN 23
#define TIMER_CUAD_LEFT 3
#define TIMER_CUAD_RIGHT 60
#define TIMER_CUAD_SPLIT 25

#define VUELTA_COMPLETA 475 // Distancia en cm de una vuelta completa
#define VUELTA_123 385  // Distancia en cm de sector 1,2 y 3
#define VUELTA_23 237
#define VUELTA_3 147


#endif
