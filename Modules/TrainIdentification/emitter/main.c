#include <avr/io.h>
#include <util/delay.h>



// Define pins
#define PIN_LED1    PB0
#define PIN_LED2    PB1

// Define the delay time in ms
#define DELAY_MS    500

// write digital "high" to pin <pn> on port <prt>
#define DIGIWRITE_H(prt, pn) prt |= (1<<pn)

// write digital "low" to pin <pn> on port <prt>
#define DIGIWRITE_L(prt, pn) prt &= ~(1<<pn)

// Define long delay function
void long_delay_ms(uint16_t ms) {
    for(ms /= 10; ms>0; ms--) _delay_ms(10);
}

// program entry point
int main(void) {
    // DDRB is the "data direction register" for port B
    // the ATtinyX5 only has port B with usable pins
    // we set both LED pins to "output"
	DDRB |= (1<<PB0); //Set pin PB0 as output
	TCNT0 = 0;
	TCCR0A=0;
	TCCR0B=0;

	TCCR0A |=(1<<COM0A0); //Timer0 in toggle mode Table 11-2
	TCCR0A |=(1<<WGM01); //Start timer 1 in CTC mode Table 11.5
	TCCR0B |= (1 << CS00);// Prescaler table 11.6
	OCR0A=104; //CTC Compare value 
	
	
	
    return 0;   /* never reached */
}
