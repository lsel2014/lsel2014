/* 
	LSEL 2014 trainID TX module
	@date March 2014
	@author Jose Martin
*/


#define ID 0b00101100
#define Carrierpin PB1

#define byteTXperiod 20

#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include "softuart.h"

// Define long delay function
void long_delay_ms(uint16_t ms) {
    for(ms /= 10; ms>0; ms--) _delay_ms(10);
}


int main(void)
{
	
	// Initialize software UART
	softuart_init();
	
	
	//Setup timer 1 compare A output as out
	DDRB |= (1 << Carrierpin);
	
	/* Configure Timer1 to generate a 36 KHz signal
	
	Preescaler = 1
	Count end = 111 (OCR1C)
	Match toggle = 111 (OCR1A)
	
	F= 8e6 / (2 * 111) = 36036 Hz
	
	*/
	TCNT1 = 0;
	TCCR1 = 0;
	
	TCCR1 |= (1 << CTC1);  // Clear on Timer Capture C match
	TCCR1 |= (1 << COM1A0); // Toggle OC1A on Capture A match
	TCCR1 |= (1 << CS10);  // Prescaler = 1
	
	GTCCR = 0;
	
	OCR1A = 111;  // Capture match value
	OCR1C = 111;  // Count clear match value

	sei();  // Enable global interrupts
	
	long_delay_ms(500);  // Completely useless...
	
	// Main loop. Just send bytes.
	for (;;) {
		_delay_ms(byteTXperiod);
		softuart_putchar(ID); 
	}
	return 0; /* never reached */
}
