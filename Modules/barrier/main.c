#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>
#include "usiTwiSlave.h"
#include <avr/interrupt.h>

// Define pins
#define PIN_LED1 PB3
#define PIN_LED2 PB4
#define PIN_PWM PB1

// Define I2C address
#define I2C_ADDRESS 0x50

// Define I2C commands for opening and closing the barrier
#define NOP 0x0F
#define OPEN_COMMAND 0x30
#define CLOSE_COMMAND 0x31
#define LIGHTS_ON 0x32

// Define I2C responses to the master
#define NOTHING 0x00
#define ACK 0x0F
#define NACK 0xFF
#define WRONG_REGISTER 0xF0

// Define states for OCR0B
#define SET_90_DEG 0x0E  //Estos valores hay que calcularlos
#define SET_0_DEG 0x07

bool semState = false;        // used to allow the lights blinking
bool ledState = false;        // used to set the LED1
long previousMillis = 0;     // will store last time LED was updated
long interval = 500;         // blinking period

uint8_t lastCommand = NOP;
bool newCommand = false;
uint8_t lastResponse = NOTHING;
bool newResponse = false;



// method for turning the blinking lights on
void lights_on(void){
	//blinking leds on
	semState = true;
}

// method for closing the barrier
void close_barrier(void){
	//set barrier to closed position
	OCR0B = SET_0_DEG;
}

// method for opening the barrier and turning off the blinking lights
void open_barrier(void){
	//blinking leds off
	semState = false;
	//set barrier to closed position
	OCR0B = SET_90_DEG;
}

// A callback triggered when the i2c master attempts to write to a register.
void i2cWriteToRegister(uint8_t reg, uint8_t value)
{
	switch (reg) {
		case 0: 
			if (!newCommand) {
				// If we're ready to receive a new command we store it
				// and prepare the ACK response
				lastCommand = value;
				newCommand = true;
				lastResponse = ACK;
				newResponse = true;
			}
			else {
				// If we're not ready to receive a new command
				// we prepare the NACK response
				lastResponse = NACK;
				newResponse = true;
			}
		break;

		default:
			// If the master writes a register different from 0
			// we prepare the WRONG_REGISTER response
			lastResponse = WRONG_REGISTER;
			newResponse = true;
		break;
	}
}

// A callback triggered when the i2c master attempts to read from a register.
uint8_t i2cReadFromRegister(uint8_t reg)
{
	switch (reg) {
		case 0: 
			if (newResponse) {
				// if a new response is prepared we send it
				newResponse = false;
				return lastResponse;
			}
			else
				// if there's not a new response prepared we send a NOTHING response
				return NOTHING;
			break;

		default:
			// we send a NOTHING response if a register different from 0 is read
			return NOTHING;
	}
}

// Initializations
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

   	//blinking leds off
	semState = false;
	ledState = false;

    // Set the I2C slave address of the controller
    usiTwiSlaveInit(I2C_ADDRESS, i2cReadFromRegister, i2cWriteToRegister);

    // enable interrupts
    sei();
}

// function in charge updating the lights
void loop() {
  unsigned long currentMillis = millis();
 
  if(currentMillis - previousMillis > interval) {
    // save the last time you blinked the LED 
    previousMillis = currentMillis;   

    if(semState) {
      if (ledState) {
        ledState = false;
        digitalWrite(PIN_LED1, 0);
        digitalWrite(PIN_LED1, 1);
      } else {
        ledState = true;
        digitalWrite(PIN_LED1, 1);
        digitalWrite(PIN_LED1, 0);
      }
    }
  }
}

// main
int main(void) { 
    _init();
    while(1) {
	 	// Wait for a command then proceed with the corresponding function
	   	if (newCommand) {
	        if (newCommand == CLOSE_COMMAND) {
			  close_barrier();
	       	} else if (newCommand == OPEN_COMMAND) {
			  open_barrier();
			} else if (newCommand == LIGHTS_ON) {
			  lights_on();
			}

			newCommand = false;
	    }

	    loop();
   }
}
