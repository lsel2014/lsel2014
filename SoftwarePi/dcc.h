#ifndef DCC_H
#define DCC_H
#define PACKET_BUFFER_SIZE 16
#define DCC_PERIOD 58000
#include <native/mutex.h>


/*
 * DCC packet structure.
 * 14bit preamble (only 4 here to fit in 32bit)
 * 1 bit packet start bit (always 0)
 * 8 bit address (0-127, MSB always 0)
 * 1 bit address-data separator (always 0)
 * 8 bit data (different formats available)
 * 1 bit data-ecc separator (always 0)
 * 8 bit error checking code (address XOR data)
 * 1 bit packet end (always 1)
 */
	typedef union{
		struct {
			unsigned char preamble		:4;
			unsigned char packet_start	:1;
			unsigned char address 		:8;
			unsigned char adseparator	:1;
			unsigned char data		:8;
			unsigned char deseparator 	:1;
			unsigned char ecc		:8;
			unsigned char packet_end    	:1;
		};
		unsigned int packet;
	}dcc_packet_t;

	typedef struct{
		dcc_packet_t packet_buffer[PACKET_BUFFER_SIZE];
		int writePointer;
		int readPointer;
		int pending_packets;
	}dcc_buffer;


	typedef struct {
		int dcc_gpio;
		int pending_packets;
		dcc_buffer buffer;
		RT_MUTEX dcc_mutex;
	}dcc_sender_t;


	void packet_init (dcc_packet_t packet);
	dcc_sender_t* dcc_new (int gpio, int prio, int deadline);
	void dcc_init(dcc_sender_t* this, int dcc_gpio, int prio, int deadline);
	void dcc_add_packet(dcc_sender_t* this, dcc_packet_t packet);
	void dcc_add_speed_packet(dcc_sender_t* this, unsigned char address, int speed);
	void dcc_send(void* arg);

#endif
