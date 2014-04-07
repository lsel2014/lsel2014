#ifndef DCC_H
#define DCC_H

typedef struct{
	int dcc_gpio;
	dcc_packet packetbuffer[MAX_PACKETS];
	char transmitting;
	RT_MUTEX dcc_mutex;
}dccstate_t;

typedef union
{
	struct
		{
		unsigned char preamble :4 =0xF;
		unsigned char packet_start:1 =1;
		unsigned char address :8;
		unsigned char separator:1 =0;
		unsigned char data:8;
		unsigned char separator2:1 =0;
		unsigned char error_detect:8;
		unsigned char packet_end:1;
		};
		unsi
		
		


}dcc_packet;

#endif