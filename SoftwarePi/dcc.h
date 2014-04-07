#ifndef DCC_H
#define DCC_H
#define PACKET_BUFFER_SIZE 16
#define DCC_PERIOD 58000
#include <native/mutex.h>
typedef enum{
	NEW_PACKET,
	PREAMBLE,
	PACKET_BODY
} dcc_state_t;

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
			unsigned char preamble		:4 =0xF;
			unsigned char packet_start	:1 =0;
			unsigned char address 		:8;
			unsigned char adseparator	:1 =0;
			unsigned char data			:8;
			unsigned char deseparator 	:1 =0;
			unsigned char ecc			:8;
			unsigned char packet_end    :1 =1;
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
		dcc_state_t state;
		RT_MUTEX dcc_mutex;
	}dcc_sender_t;

	dcc_sender_t* dcc_new (int gpio, int prio);
	dcc_init(dcc_sender_t* this, int dcc_gpio, int prio);
	dcc_add_packet(dcc_sender_t* this, dcc_packet_t packet);
	dcc_add_speed_packet(dcc_sender_t* this, int speed);
	dcc_send(void* arg);

#endif
