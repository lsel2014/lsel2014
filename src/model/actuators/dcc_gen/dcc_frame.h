#ifndef DCC_FRAME_H
#define DCC_FRAME_H

#include "dcc_const.h"

typedef enum dcc_frame_type_t
{
    IDLE,
    NORMAL,
    RESET_ALL,
    BROADCAST_STOP

} dcc_frame_type_t;

typedef char dcc_frame_address_t;
typedef char dcc_frame_data_t;

/*
 * DCC frame
 */
typedef struct dcc_frame_t
{
    //Type
    dcc_frame_type_t type;
    //Address
    dcc_frame_address_t address;
    //Data
    dcc_frame_data_t data;
    //Error check
    dcc_frame_data_t error;

} dcc_frame_t;


#endif
