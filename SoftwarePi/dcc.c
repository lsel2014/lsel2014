#include "dcc.h"
#include <native/mutex.h>
#include <native/task.h>
#include <stdlib.h>
#include "daemon.h"
#include "tasks.h"
#include <wiringPi.h>

const char train_speed_codes[29] = { 0b00000, 0b00010, 0b10010, 0b00011,
		0b10011, 0b00100, 0b10100, 0b00101, 0b10101, 0b00110, 0b10110, 0b00111,
		0b10111, 0b01000, 0b11000, 0b01001, 0b11001, 0b01010, 0b11010, 0b01011,
		0b11011, 0b01100, 0b11100, 0b01101, 0b11101, 0b01110, 0b11110, 0b01111,
		0b11111 };

void packet_init (dcc_packet_t packet){
	packet.preamble = 0xF;
	packet.packet_start = 0;
	packet.adseparator = 0; 
	packet.deseparator = 0;
	packet.packet_end = 1;
}

dcc_sender_t* dcc_new(int gpio, int prio, int deadline) {
	dcc_sender_t* this = (dcc_sender_t*) malloc (sizeof(dcc_sender_t));
	dcc_init(this, gpio, deadline);
	return this;
}

void dcc_init (dcc_sender_t* this, int dcc_gpio, int deadline) {
	pinMode(dcc_gpio, OUTPUT);
	this->dcc_gpio = dcc_gpio;
	this->pending_packets = 0;
	this->buffer.readPointer = 0;
	this->buffer.writePointer = 0;
	this->buffer.pending_packets = 0;
	rt_mutex_create(&(this->dcc_mutex), "dcc_mutex");
	task_add("DCC sender",63000,dcc_send,this);

}
void dcc_add_packet(dcc_sender_t* this, dcc_packet_t packet) {
	/*
	 * Circular buffer with overtake prevention. Old packets get discarded if
	 * buffer is full;
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
}

void dcc_add_speed_packet(dcc_sender_t* this, unsigned char address, int speed) {
	dcc_packet_t dcc_packet;
	packet_init (dcc_packet);
	dcc_packet.address = address;
	if (speed>0) {
		dcc_packet.data = 0b01100000 | train_speed_codes[abs(speed)];
	} else {
		dcc_packet.data = 0b01000000 | train_speed_codes[abs(speed)];
	}
	dcc_packet.ecc = dcc_packet.address ^ dcc_packet.data;
	dcc_add_packet(this, dcc_packet);
}
;

void dcc_send(void* arg) {
	dcc_sender_t* this = (dcc_sender_t*) arg;
	static dcc_packet_t current_packet;
	unsigned char buffer_bit = 0xFF;
	unsigned long long buffer = 0;
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
							| (((unsigned long long) current_packet.packet) << 22);
					idle_packet = buffer;
				} else {
					buffer = idle_packet;
				}
			}
			buffer_bit = (buffer & 0x8000000000000000ULL) ? 0xFD : 0xF3;
			buffer = buffer << 1;
		}
		digitalWrite(this->dcc_gpio, (buffer_bit & 0x01));
		buffer_bit = buffer_bit >> 1;
		buffer_bit = buffer_bit | 0x80;
	}
}
;

