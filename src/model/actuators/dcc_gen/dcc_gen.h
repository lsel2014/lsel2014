#ifndef DCC_GEN_H
#define DCC_GEN_H

#include "dcc_frame.h"
#include <native/mutex.h>

#define FRAME_BUFFER_SIZE 64

/*
 * Command buffer
 */
typedef struct dcc_buffer_t
{
    dcc_frame_t frame_buffer [FRAME_BUFFER_SIZE];

    int write_pointer;
    int read_pointer;
    int pending_frames;

}


/*
 * Frame generator
 */
typedef struct dcc_gen_t
{
    dcc_buffer_t buffer;
    int dcc_gpio;

    RT_MUTEX dcc_mutex;

}

dcc_sender_t* dcc_new(int gpio, int deadline);
void dcc_init(dcc_sender_t* this, int dcc_gpio, int deadline);
void dcc_add_packet(dcc_sender_t* this, dcc_packet_t packet);
void dcc_add_function_packet(dcc_sender_t* this, unsigned char address, unsigned char function, unsigned char state);
void dcc_add_speed_packet(dcc_sender_t* this, unsigned char address, int speed);
void dcc_add_data_packet(dcc_sender_t* this, unsigned char address, unsigned char data);
void dcc_send(void* arg);





#endif
