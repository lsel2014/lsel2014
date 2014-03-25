/* 
	LSEL 2014 trainID RX module
	@date March 2014
	@author Jose Martin
*/


#define ID1 0b00110100
#define ID1pin PB0

#define ID2 0b00101100
#define ID2pin PB2


#include <avr/interrupt.h>
#include <util/delay.h>
#include "softuart.h"

// write digital "high" to pin <pn> on port <prt>
#define DIGIWRITE_H(prt, pn) prt |= (1<<pn)

// write digital "low" to pin <pn> on port <prt>
#define DIGIWRITE_L(prt, pn) prt &= ~(1<<pn)

// Define long delay function
void long_delay_ms(uint16_t ms) {
    for(ms /= 10; ms>0; ms--) _delay_ms(10);
}


int main(void)
{
	
	// Initialize software UART
	softuart_init();
	
	//Setup output pins as outs
	DDRB |= (1 << PB2);
	DDRB |= (1 << PB0);
	
	
	/* Configure Timer1 to put LOW on output lines after 100 ms
	
	Preescaler = 16384
	Count end = 49 (OCR1C)
	Match interrup = 49 (OCR1A)
	
	F= 8e6 / (16384 * 49) = 10 Hz
	
	*/
	TCNT1 = 0;
	TCCR1 = 0;
	GTCCR = 0;
	
	TCCR1 |= (1 << CTC1); // Clear on Timer Capture C match
	TCCR1 |= (1 << CS13) | (1 << CS12) | (1 << CS11) | (1 << CS10); // Prescaler 16384
	
	OCR1A = 49; // Capture match value
	OCR1C = 49; // Count clear match value
	
	TIMSK |= (1 << OCIE1A); // Activate Timer 1 Compare A interrupt

	sei();   // Enable global interrupts
	
	long_delay_ms(500);  // Completely useless...

	// Main loop, receive bytes all the time
	for (;;) {
		uint8_t byte = softuart_getchar();  // Waits until a new byte is received

		// Compare against the two implented trains. Timer 1 count is reset to make sure that the pin will remain the 100 ms HIGH
		if (byte == ID1) {
			DIGIWRITE_H(PORTB, ID1pin);
			TCNT1=0;
		}
		else  if (byte == ID2) {
			DIGIWRITE_H(PORTB, ID2pin);
			TCNT1=0;
		}
	}
	return 0; /* never reached */
}

/* Timer 1 Compare A interrupt.
Clears the two output lines */
ISR(TIM1_COMPA_vect)
{
	DIGIWRITE_L(PORTB,ID1pin);
	DIGIWRITE_L(PORTB,ID2pin);
} 
