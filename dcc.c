/**
 * @file    dcc.c
 * @date    --
 * @brief   DCC entity definition
 *
 * Implementation of dcc_sender_t functions and declarations
 *
 * This is an implementation of a complete DCC sender for the Raspberry Pi.
 * These functions emulate the basic DCC functionality. Support for extended
 * packets may be added in the future.
 *
 * Signals are generated via bit-banging. Timing is controlled by a periodic
 * real-time task that toggles the sending GPIO.
 */

#include <native/mutex.h>
#include <native/task.h>
#include <stdlib.h>
#include <rtdk.h>
#include <wiringPi.h>

#include "dcc.h"
#include "daemon.h"
#include "tasks.h"

/**
 * Speed codes defined by DCC standard (28 step speed format). Mapped for convenience.
 */
const char train_speed_codes[29] = { 0b00000, 0b00010, 0b10010, 0b00011,
		0b10011, 0b00100, 0b10100, 0b00101, 0b10101, 0b00110, 0b10110, 0b00111,
		0b10111, 0b01000, 0b11000, 0b01001, 0b11001, 0b01010, 0b11010, 0b01011,
		0b11011, 0b01100, 0b11100, 0b01101, 0b11101, 0b01110, 0b11110, 0b01111,
		0b11111 };


// Object creation/destruction -----

/**
 * @brief dcc_t creation
 *
 * Creates a dcc_sender_t object
 *
 * @ingroup dcc_t_object
 *
 * @param gpio      GPIO port to send packets through
 * @param deadline  Periodic task deadline
 *
 * @returns this    Malloc'd and initialized dcc_sender_t object
 */
dcc_sender_t* dcc_new(int dcc_gpiop, int dcc_gpion, int deadline)
{
	dcc_sender_t* this = (dcc_sender_t*) malloc(sizeof(dcc_sender_t));
	dcc_init(this, dcc_gpiop,dcc_gpion, deadline);
	return this;
}

/**
 * @brief dcc_sender_t initialization
 *
 * @ingroup dcc_t_object
 *
 * @param this      Malloc'd dcc_sender_t object
 * @param dcc_gpio  GPIO port to send packets through
 * @param deadline  Periodic task deadline
 */
void dcc_init(dcc_sender_t* this, int dcc_gpiop, int dcc_gpion, int deadline)
{
	pinMode(dcc_gpio, OUTPUT);
	this->dcc_gpiop = dcc_gpiop;
	this->dcc_gpion = dcc_gpion;
	this->pending_packets = 0;
	this->buffer.readPointer = 0;
	this->buffer.writePointer = 0;
	this->buffer.pending_packets = 0;
	rt_mutex_create(&(this->dcc_mutex), NULL);
	task_add("DCC sender", DCC_DEADLINE, dcc_send, this);

}


// Actions of dcc_sender_t -----

/**
 * @brief Add packet to DCC buffer
 *
 * Adds a packet to the DCC packet queue. Packets are serviced in a FIFO fashion.
 * If the queue is full old packets will be discarded to make room for new ones.
 *
 * @ingroup dcc_t_actions
 *
 * @param this the dcc_sender that will transmit the packet
 * @param packet the packet to add
 */
void dcc_add_packet(dcc_sender_t* this, dcc_packet_t packet)
{
	/*
	 * Circular buffer with overtake prevention. Old packets get discarded if
	 * buffer is full
	 */
	rt_mutex_acquire(&(this->dcc_mutex), TM_INFINITE);
	this->buffer.packet_buffer[this->buffer.writePointer] = packet;
	this->buffer.writePointer++;
	this->buffer.writePointer %= PACKET_BUFFER_SIZE;
	if (this->buffer.writePointer == this->buffer.readPointer) {
		this->buffer.readPointer++;
		this->buffer.readPointer %= PACKET_BUFFER_SIZE;
	} else {
		this->buffer.pending_packets++;
	}
	rt_mutex_release(&(this->dcc_mutex));
	//rt_printf("%x\n", packet);
}

/**
 * @brief Add speed packet to DCC buffer
 *
 * Formats a 28-step DCC speed packet including direction and adds it to the
 * sending queue.
 *
 * @ingroup dcc_t_actions
 *
 * @param this the dcc_sender that will transmit the packet
 * @param address 7 bit address of the target receiver
 * @param speed desired train speed between -28 (top backwards speed) to 28 (top forward speed)
 */
void dcc_add_speed_packet(dcc_sender_t* this, unsigned char address, int speed) {
	dcc_packet_t dcc_packet;
	char dcc_packet_data, dcc_packet_ecc;
	dcc_packet = 0b11110000000000000000000000000001;
	dcc_packet |= ((unsigned int) address) << 19;
	if (speed > 28)
		speed = 28;
	if (speed < -28)
		speed = -28;
	if (speed > 0) {
		dcc_packet_data = 0b01100000 | train_speed_codes[abs(speed)];
	} else {
		dcc_packet_data = 0b01000000 | train_speed_codes[abs(speed)];
	}
	dcc_packet |= ((unsigned int) dcc_packet_data) << 10;
	dcc_packet_ecc = address ^ dcc_packet_data;
	dcc_packet |= ((unsigned int) dcc_packet_ecc) << 1;
	dcc_add_packet(this, dcc_packet);
}

/**
 * @brief Add data packet to DCC buffer
 *
 * Formats a generic basic DCC packet with 7 bit address and 8 bit data, filling
 * the preamble and separator bits and calculating the required CRC. Then adds
 * it to the packet queue for transmission.
 *
 * @ingroup dcc_t_actions
 *
 * @param this the dcc_sender that will transmit the packet
 * @param address 7 bit address of the target receiver
 * @param data 8 bit generic data
 */
void dcc_add_data_packet(dcc_sender_t* this, unsigned char address,
		unsigned char data)
{
	dcc_packet_t dcc_packet;
	char dcc_packet_ecc;
	dcc_packet = 0b11110000000000000000000000000001;
	dcc_packet |= ((unsigned int) address) << 19;
	dcc_packet |= ((unsigned int) data) << 10;
	dcc_packet_ecc = address ^ data;
	dcc_packet |= ((unsigned int) dcc_packet_ecc) << 1;
	dcc_add_packet(this, dcc_packet);
}

/*
 * @brief Add function packet to DCC buffer
 *
 * Formats a basic function DCC packet. These packets are sent when the
 * user toggles a DCC basic function (F0-F14), which are mapped on each receiver
 * to control functions such as train lights, sound, actuators...
 * Only functions F0 to F12 are supported and only one of each group (1-4, 5-8 or 9-12)
 * can be enabled at a time with this implementation.
 *
 * Read the DCC standard for further information.
 *
 * @ingroup dcc_t_actions
 * 
 * @param this the dcc_sender that will transmit the packet
 * @param address 7 bit address of the target receiver
 * @param function 0 to 12 indicates the function number to modify
 * @param state 1 enables the function, 0 disables it
 */
void dcc_add_function_packet(dcc_sender_t* this, unsigned char address,
		unsigned char function, unsigned char state)
{
	dcc_packet_t dcc_packet;
	char dcc_packet_ecc;
	char data = 0;
	if (function < 5) {
		data |= 0b10000000;
		if (state) {
			data |= 1 << (function - 1);
		}
	}
	if (function >= 5 && function < 9) {
		data |= 0b10110000;
		if (state) {
			data |= 1 << (function - 5);
		}
	}
	if (function >= 9 && function < 12) {
		data |= 0b10100000;
		if (state) {
			data |= 1 << (function - 9);
		}
	}

	dcc_packet = 0b11110000000000000000000000000001;
	dcc_packet |= ((unsigned int) address) << 19;
	dcc_packet |= ((unsigned int) data) << 10;
	dcc_packet_ecc = address ^ data;
	dcc_packet |= ((unsigned int) dcc_packet_ecc) << 1;
	dcc_add_packet(this, dcc_packet);
}


// Tasks of dcc_sender_t -----

/**
 * @brief Periodic task associated to every dcc_sender
 *
 * This periodic task is used for bit-banging the DCC protocol. Packets from
 * the sending queue are taken in a FIFO fashion and sent. If no new packets
 * are available the last transmitted packet is repeated until a new one
 * arrives. This auto-retransmission feature improves packet loss rates in
 * some scenarios.
 *
 * @ingroup dcc_t_tasks
 *
 * @param arg The dcc_sender associated to the task.
 */
void dcc_send(void* arg) {
	dcc_sender_t* this = (dcc_sender_t*) arg;
	static dcc_packet_t current_packet;
	unsigned char buffer_bit = 0xFF;
	unsigned long long buffer = 0;
	/*Idle DCC packet. Refer to DCC standard for further information*/
	unsigned long long idle_packet = 0xFFFDFE007FC00000ULL;
	rt_task_set_periodic(NULL, TM_NOW, DCC_PERIOD);
	while (1) {
		rt_task_wait_period(NULL);
		if (buffer_bit == 0xFF) {
			if (buffer == 0) {
				if (this->buffer.pending_packets) {
					rt_mutex_acquire(&(this->dcc_mutex), TM_INFINITE);
					current_packet =
							this->buffer.packet_buffer[this->buffer.readPointer];
					this->buffer.readPointer++;
					this->buffer.readPointer %= PACKET_BUFFER_SIZE;
					this->buffer.pending_packets--;
					rt_mutex_release(&(this->dcc_mutex));
					buffer = 0xFFFC000000000000ULL
							| (((unsigned long long) current_packet) << 22);
					idle_packet = buffer;
				} else {
					buffer = idle_packet;
				}
			}
			/*A logic 1 is sent as a 58us high/58us low cycle
			 * 0xFD = 0b11111101
			 * A logic 0 is sent as a 116us high/116us low cycle
			 * 0xF3 = 0b11110011
			 * Bits are shifted every 58us
			 */
			buffer_bit = (buffer & 0x8000000000000000ULL) ? 0xFD : 0xF3;
			buffer = buffer << 1;
		}
		digitalWrite(this->dcc_gpiop, (buffer_bit & 0x01));
		digitalWrite(this->dcc_gpion, (buffer_bit & 0x01)?0:1);
		buffer_bit = buffer_bit >> 1;
		buffer_bit = buffer_bit | 0x80;
	}
}

