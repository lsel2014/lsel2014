/************************************************************************
 *                                                                      
 * LSEL2014 sector semaphore module
 *
 * @date 26/05/2014 21:16:49
 * @author Javier Gonzalez
 *
 *                             AT tiny 45
 *                 +--------------------------------+
 * Reset		   | 1 pb5 reset              VCC 8 |
 * Green LED	   | 2 pb3                SCL pb2 7 | SCL
 * Red LED		   | 3 pb4 	                  pb1 6 | Yellow LED
 *                 | 4 GND                SDA pb0 5 | SDA
 *                 +--------------------------------+
 ************************************************************************/ 


#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "usiTwiSlave.h" 

#define I2C_SLAVE_ADDR 0x33
#define PIN_G PB3
#define PIN_R PB4
#define PIN_Y PB1

// write digital "high" to pin <pn> on port <prt>
#define DIGIWRITE_H(prt, pn) prt |= (1<<pn)

// write digital "low" to pin <pn> on port <prt>
#define DIGIWRITE_L(prt, pn) prt &= ~(1<<pn)

volatile char data;

uint8_t i2cReadFromRegister(uint8_t reg)
{
	switch (reg)
	{
	}
	return 0xff;
}

void i2cWriteToRegister(uint8_t reg, uint8_t value)
{
	switch (reg)
	{
		        case 0:
		        	if (value == 1){				//Green
						DIGIWRITE_H(PORTB,PIN_G);
						DIGIWRITE_L(PORTB,PIN_Y);
						DIGIWRITE_L(PORTB,PIN_R);
						data = 1;
					}else if (value == 2){			//Yellow
						DIGIWRITE_L(PORTB,PIN_G);
						DIGIWRITE_H(PORTB,PIN_Y);
						DIGIWRITE_L(PORTB,PIN_R);
						data = 2;
					}else if (value == 3){			//Red
						DIGIWRITE_L(PORTB,PIN_G);
						DIGIWRITE_L(PORTB,PIN_Y);
						DIGIWRITE_H(PORTB,PIN_R);
						data = 3;
					}else{							//Error
						DIGIWRITE_H(PORTB,PIN_G);
						DIGIWRITE_H(PORTB,PIN_Y);
						DIGIWRITE_H(PORTB,PIN_R);
						data = 0;
					}
					break;		
				default:			//All of them, just if something goes wrong
					DIGIWRITE_H(PORTB,PIN_G);
					DIGIWRITE_H(PORTB,PIN_Y);
					DIGIWRITE_H(PORTB,PIN_R);
					data = 0;
					break;
        }
		
		
}


void setup(void)
{	
	usiTwiSlaveInit(I2C_SLAVE_ADDR, i2cReadFromRegister, i2cWriteToRegister);		//Init I2C Slave mode
	
	sei();
	
	DDRB |= (1<<PIN_G);
	DDRB |= (1<<PIN_Y);
	DDRB |= (1<<PIN_R);						//Pins 2, 3 and 6 (pb1, 2 and 4) as outputs
	
	
	DIGIWRITE_L(PORTB,PIN_G);
	DIGIWRITE_L(PORTB,PIN_Y);
	DIGIWRITE_L(PORTB,PIN_R);
	
}

int main(void)
{
	data = 0;
	setup();
	while (1){
		if (data == 3){
			_delay_ms(500);
			DIGIWRITE_L(PORTB,PIN_R);
			_delay_ms(500);
			DIGIWRITE_H(PORTB,PIN_R);
		}	
	};
	return 0; // Never reached
    
}
