/*
	LSEL 2014 barrier and semaphore controller
	@date June 2014


*/


#define I2C_ADDRESS 0x69 // Address must be different for each thing

#define UP 1
#define DOWN 0

#define LEDS_A 1
#define LEDS_B 2
#define LEDS_OFF 0

// Define pins
#define PIN_LED1 PB3
#define PIN_LED2 PB4
#define PIN_PWM PB1

// Define states for OCR0B
#define SET_0_DEG 0x0D  //Estos valores hay que calcularlos
#define SET_90_DEG 0x06


// write digital "high" to pin <pn> on port <prt>
#define DIGIWRITE_H(prt, pn) prt |= (1<<pn)

// write digital "low" to pin <pn> on port <prt>
#define DIGIWRITE_L(prt, pn) prt &= ~(1<<pn)

#include <avr/interrupt.h>
#include <util/delay.h>
#include "usiTwiSlave.h"

volatile int done = 0;
volatile int leds_state = LEDS_OFF;

// Define long delay function
void long_delay_ms(uint16_t ms) {
    for(ms /= 10; ms>0; ms--) _delay_ms(10);
}

// A callback triggered when the i2c master attempts to read from a register.
uint8_t i2cReadFromRegister(uint8_t reg)
{
	switch (reg)
	{
		case 0: 
			if (OCR0B == SET_90_DEG)
			{
				return UP;
			}
			else {
				if (OCR0B == SET_0_DEG)
				{
					return DOWN;
				}
			}
			break;
		case 1:
			return leds_state;
			break;
		default:
			return 0xff;
			break;
	}
}

// A callback triggered when the i2c master attempts to write to a register.
void i2cWriteToRegister(uint8_t reg, uint8_t value)
{
	switch (reg)
	{
		case 0: 
			if (value == UP)
			{
				OCR0B = SET_90_DEG;
				DDRB |= (1 << PIN_PWM);
				done = 0;
			}
			else {
				if (value == DOWN)
				{
					OCR0B = SET_0_DEG;
					DDRB |= (1 << PIN_PWM);
					done = 0;
				}
			}
				
			break;
			
		case 1: 
			if (value == LEDS_A)
			{
				DIGIWRITE_H(PORTB,3);
				DIGIWRITE_L(PORTB,4);
				leds_state = LEDS_A;
			}
			else {
				if (value == LEDS_B)
				{
					DIGIWRITE_H(PORTB,4);
					DIGIWRITE_L(PORTB,3);
					leds_state = LEDS_B;
				}
				else 
				{
					DIGIWRITE_L(PORTB,3);
					DIGIWRITE_L(PORTB,4);
					leds_state = LEDS_OFF;
				}
			}
			
			break;
	}
}





int main(void)
{

	
	
	// Configure the direction register of port B
    // 1 << PIN_LED1 Sets PIN_LED1 as an output
    // 1 << PIN_LED2 Sets PIN_LED2 as an output
    // 1 << PIN_PWM Sets PIN_PWM as an output
    DDRB |= (1 << PIN_LED1) | (1 << PIN_LED2) | (1 << PIN_PWM);

    /*
    Control Register A for Timer/Counter-0
    0<<COM0A0: prevents Timer0 from using pin OC0A
    2<<COM0B0: for non-inverted PWM
    3<<WGM00: enables Fast PWM mode (when combined with WGM02 from TCCR0B below)
    */
    TCCR0A = 0<<COM0A0 | 2<<COM0B0 | 3<<WGM00;
    
    /*
    Control Register B for Timer/Counter-0
    1<<WGM02: enables Fast PWM mode (when combined with WGM00 and WGM01 from TCCR0A above)
    5<<CS00: in order to use prescaler N = 1024
    */
    TCCR0B = 1<<WGM02 | 5<<CS00;

    /*
    Output Compare Registers
	With WGM02 set to 1 (WGM0 = 7) the register OCR0A will set TOP of the count and the
	register ACR0B will be the value compared to the count (TCNT0)
    */
    OCR0A = 0x9C; 
    OCR0B = SET_90_DEG;	
		
		
	
	/* Configure Timer1 to generate a 36 KHz signal
	
	Preescaler = 1
	Count end = 111 (OCR1C)
	Match toggle = 111 (OCR1A)
	
	F= 8e6 / (2 * 111) = 36036 Hz
	
	
	TCNT1 = 0;
	TCCR1 = 0;
	
	TCCR1 |= (1 << PWM1A); // Toggle OC1A on Capture A match
	TCCR1 |= (1 << CS00);  
	TCCR1 |= (1 << CS01); 
	TCCR1 |= (1 << CS10); 
	TCCR1 |= (1 << CS11);  
	
	GTCCR = 0;
	
	OCR1C = 127;  // Count clear match value
	*/
		
	
	
	// Initialize I2C module
	usiTwiSlaveInit(I2C_ADDRESS, i2cReadFromRegister, i2cWriteToRegister);
	sei();
	// Main loop, receive bytes all the time
	for (;;) {
		
		
		if (done == 0)
		{
			long_delay_ms(1500);
			DDRB &= ~(1<< PIN_PWM);
		}
	}
	return 0; /* never reached */
}





