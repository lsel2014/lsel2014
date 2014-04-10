#include "dcc_frame.h"

/*
 * DCC frame creation function
 *
 * Type:        IDLE | NORMAL | RESET_ALL | BROADCAST_STOP
 *
 * -----------------------------------------------------------------------------
 *
 * IDLE frames:
 *      Address:    *any* | NULL
 *      Data:       *any* | NULL
 *
 * NORMAL frames:
 *      Address:    1..253
 *      Data:       (FORWARD | REVERSE) & (S1..S28 | STOP | STOPI | ESTOP | ESTOPI)
 *
 * RESET_ALL
 *      Address:    *any* | NULL
 *      Data:       *any* | NULL
 *
 * BROADCAST_STOP
 *      Address:    *any* | NULL
 *      Data:       (FORWARD | REVERSE) & (STOP | STOPI | ESTOP | ESTOPI)
 *
 * -----------------------------------------------------------------------------
 *
 * Example: Send normal frame to address number 3 -> Move forward at speed 20
 *
 *          dcc_frame_create (NORMAL, 3, FORWARD & S20);
 *
 */
dcc_frame_t dcc_frame_create (dcc_frame_type_t type, dcc_frame_address_t address,
                                    dcc_frame_data_t data)
{
    dcc_frame_t frame;
    frame->type = type;

    switch (type)
    {
        case IDLE:
            frame->address = 0b11111111;
            frame->data    = 0b00000000;
            frame->error   = 0b11111111;
            break;

        case NORMAL:
            switch (address)
            {
                case 0b00000000:    //Restricted addresses
                case 0b11111110:
                case 0b11111111:
                    return dcc_frame_create (IDLE, NULL, NULL);  //If restricted, send IDLE
                
                default:
                    frame->address = address;
                    break;
            }
            frame->data    = data;
            frame->error   = frame->address ^ frame->data;   //XOR
            break;

        case RESET_ALL:
            frame->address = 0b00000000;
            frame->data    = 0b00000000;
            frame->error   = 0b00000000;
            break;

        case BROADCAST_STOP:
            frame->address = 0b00000000;
            switch (data & 0b011111)
            {
                case STOP:
                case STOPI:
                case ESTOP:
                case ESTOPI:
                    frame->data = data;
                    break;

                default:        //If a stop signal is not passed as a parameter, send IDLE
                    return dcc_frame_create (IDLE, NULL, NULL);
            }
            frame->error   = frame->address ^ frame->data;   //XOR
            break;

    }

    return frame;
}
