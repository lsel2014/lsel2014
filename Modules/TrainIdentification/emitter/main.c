/* 
	AVR Software-Uart Demo-Application 
	Version 0.4, 10/2010
	
	by Martin Thomas, Kaiserslautern, Germany
	<eversmith@heizung-thomas.de>
	http://www.siwawi.arubi.uni-kl.de/avr_projects
*/


//#define ID 0b01000011
#define ID 'C'

#define txVersion "1.0b"

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
	softuart_init();
	softuart_turn_rx_on(); /* redundant - on by default */
	
	
	DDRB |= (1<<PB1);
	TCNT1 = 0;
	TCCR1 = 0;
	
	TCCR1 |= (1 << CTC1);
	TCCR1 |= (1 << COM1A0);
	TCCR1 |= (1 << CS10);
	
	GTCCR = 0;
	
	OCR1A = 111;
	
	sei();
	long_delay_ms(500);

	softuart_puts_P( "\r\nLSEL 2014 trainID TX module\r\n" );
	softuart_puts_P( "Version: " txVersion "\r\n");
	
	for (;;) {
		_delay_ms(20);
		softuart_putchar(ID); 
	}
	return 0; /* never reached */
}
