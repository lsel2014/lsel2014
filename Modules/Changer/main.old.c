#include <avr/io.h>
#include <util/delay.h>
//#include "usiTwiSlave.h"
#include <avr/interrupt.h>

// Define pins
#define PIN_LED1 PB3
#define PIN_LED2 PB4
#define PIN_PWM PB1

// Define I2C address
#define I2C_ADDRESS 0x57

// Define I2C commands for opening and closing the barrier
#define OPEN_COMMAND 0x30
#define CLOSE_COMMAND 0x31
#define LIGHTS_ON 0x32

// Define states for OCR0B
#define SET_90_DEG 0x0E  //Estos valores hay que calcularlos
#define SET_0_DEG 0x07

// Define the cpu frequency
#define F_CPU 8000000

// write digital "high" to pin <pn> on port <prt>
#define DIGIWRITE_H(prt, pn) prt |= (1<<pn)

// write digital "low" to pin <pn> on port <prt>
#define DIGIWRITE_L(prt, pn) prt &= ~(1<<pn)

int semState = 0;
int ledState = 0;             // ledState used to set the LED
long previousMillis = 0;        // will store last time LED was updated
long interval = 500;

// Define long delay function
void long_delay_ms(uint16_t ms) {
    for(ms /= 10; ms>0; ms--) _delay_ms(10);
}

// method for turning the blinking lights on
void lights_on(void){
	//blinking leds on
	GTCCR = 1<<PWM1B;
}

// method for closing the barrier
void close_barrier(void){
	//set barrier to closed position
	OCR0B = SET_0_DEG;
}

// method for opening the barrier and turning off the blinking lights
void open_barrier(void){
	//blinking leds off
	GTCCR = 0<<PWM1B;
	//set barrier to closed position
	OCR0B = SET_90_DEG;
}

void _init(){
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

    // Set the I2C slave address of the controller
    //usiTwiSlaveInit(I2C_ADDRESS);

    // enable interrupts
    //sei();
}

void loop() {
  unsigned long currentMillis = millis();
 
  if(currentMillis - previousMillis > interval) {
    // save the last time you blinked the LED 
    previousMillis = currentMillis;   

    if(semState ==1) {
      if (ledState == 0) {
        ledState = 1;
        digitalWrite(PIN_LED1, 1);
        digitalWrite(PIN_LED1, 0);
      } else {
        ledState = 0;
        digitalWrite(PIN_LED1, 0);
        digitalWrite(PIN_LED1, 1);
      }
    }
  }
}

int main(void) {
    uint8_t command; 
    _init();
//    while(1) {
//  	// Wait for a command then proceed with the corresponding function
//    	if (usiTwiDataInReceiveBuffer()) {
//		command = usiTwiReceiveByte();
//           	if (command == CLOSE_COMMAND) {
//		  close_barrier();
//           	} else if (command == OPEN_COMMAND) {
//		  open_barrier();
//		} else if (command == LIGHTS_ON) {
//		  lights_on();
//		}
//        }
//    }
    while(1){
       long_delay_ms(2000);
       open_barrier();
       long_delay_ms(2000);
       close_barrier();
    }
}
