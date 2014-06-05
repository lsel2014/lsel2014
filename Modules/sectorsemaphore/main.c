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

#define I2C_SLAVE_ADDR 0x30
#define PIN_G 3
#define PIN_R 4
#define PIN_Y 1

uint8_t data;

void setup(void)
{
	sei();
	
	usiTwiSlaveInit(I2C_SLAVE_ADDR);		//Init I2C Slave mode
	
	DDRB |= (1<<DDB4);
	DDRB |= (1<<DDB3);
	DDRB |= (1<<DDB1);						//Pins 2, 3 and 6 (pb1, 2 and 4) as outputs
	
	PORTB &= (0<<PB4);
	PORTB &= (0<<PB3);
	PORTB &= (0<<PB1);			//Initialize to 0
	data = 0;
}

void blink(void)
{
	_delay_ms(500);
	PORTB &= (0<<PB4);
	_delay_ms(500);
	PORTB |= (1<<PB4);
}

int main(void)
{
	uint8_t byteRcvd = 0;
	setup();
	
    while(1)
    {
        if(usiTwiDataInReceiveBuffer()) {
        	byteRcvd = usiTwiReceiveByte();     // get the byte from master
        	
	        switch (byteRcvd)							// the case is selected by a single
	        {											// digit in the master code. (1,2 or 3)
		        case 1:				//Green
					data = 1;
					PORTB &= (0<<PB4);
					PORTB |= (1<<PB3);
					PORTB &= (0<<PB1);
					break;		
		        case 2:				//Yellow
					data = 2;
					PORTB &= (0<<PB4);
					PORTB &= (0<<PB3);
					PORTB |= (1<<PB1);
					break;
		        case 3:				//Red
					data = 3;
					PORTB |= (1<<PB4);
					PORTB &= (0<<PB3);
					PORTB &= (0<<PB1);
					break;
				default:			//All of them, just if something goes wrong
					data = 99;
					PORTB |= (1<<PB4);
					PORTB |= (1<<PB3);
					PORTB |= (1<<PB1);
					break;
	        }

        }
		
		if (data == 3)
		{
			blink();
		}
		
    }
    
    return 1; //should never be reached
}
