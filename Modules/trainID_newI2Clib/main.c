/*
	LSEL 2014 i2c ref example
	@date March 2014
	@author Jose Martin


*/


#define I2C_ADDRESS 0x24 // Address must be different for each thing

#include <avr/interrupt.h>
#include <util/delay.h>
#include "softuart.h"
#include "usiTwiSlave.h"

static uint8_t byte = 0;

// Somewhere to store the values the master writes to i2c register 2 and 3.
static volatile uint8_t i2cReg0 = 0;
static volatile uint8_t i2cReg1 = 0;

// Duplicates a byte to prevent errors
uint8_t bufferData (uint8_t data){
	uint8_t bufferedByte = data;
	return bufferedByte;
}

// A callback triggered when the i2c master attempts to read from a register.
uint8_t i2cReadFromRegister(uint8_t reg)
{
	switch (reg)
	{
		case 0: 
			return bufferData(byte); // Buffers the byte received from TX before sending it
			break;
		case 1:
			return i2cReg1;
			break;
		default:
			return 0xff;
	}
}

// A callback triggered when the i2c master attempts to write to a register.
void i2cWriteToRegister(uint8_t reg, uint8_t value)
{
	switch (reg)
	{
		case 0: 
			i2cReg0 = value;
			break;
		case 1:
			i2cReg1 = value;
			break;
	}
}
	
int main(void)
{
	// Initialize software UART - remove if not needed
	softuart_init();
	
	// Initialize I2C module
	usiTwiSlaveInit(I2C_ADDRESS, i2cReadFromRegister, i2cWriteToRegister);
	
	sei();
	
	// Main loop, receive bytes all the time
	for (;;) {
		if (softuart_kbhit() != 0){	// Checks if a new byte is received from TX and stores it
			byte = softuart_getchar();  
		}	
	}
	return 0; /* never reached */
}
