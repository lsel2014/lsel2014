#ifndef DCC_H
#define DCC_H
#define PACKET_BUFFER_SIZE 32
#define DCC_PERIOD 58000
#define DCC_DEADLINE 63000 //DCC tolerates up to 63us half period
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
typedef unsigned int dcc_packet_t;

typedef struct {
	dcc_packet_t packet_buffer[PACKET_BUFFER_SIZE];
	int writePointer;
	int readPointer;
	int pending_packets;
} dcc_buffer;

typedef struct {
	int dcc_gpio;
	int pending_packets;
	dcc_buffer buffer;
	RT_MUTEX dcc_mutex;
} dcc_sender_t;

dcc_sender_t* dcc_new(int gpio, int deadline);
void dcc_init(dcc_sender_t* this, int dcc_gpio, int deadline);
void dcc_add_packet(dcc_sender_t* this, dcc_packet_t packet);
void dcc_add_function_packet(dcc_sender_t* this, unsigned char address,
		unsigned char function, unsigned char state);
void dcc_add_speed_packet(dcc_sender_t* this, unsigned char address, int speed);
void dcc_add_data_packet(dcc_sender_t* this, unsigned char address,
		unsigned char data);
void dcc_send(void* arg);

#endif
