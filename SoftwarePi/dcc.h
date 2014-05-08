/**
 * @file    dcc.h
 * @date    --
 * @brief   DCC entity declaration
 *
 * Declaration of dcc_sender_t object and functions
 */

/**
 * DCC entity
 *
 * @defgroup DCC
 * @{
 */

#ifndef DCC_H
#define DCC_H

/**
 * DCC packet structure:
 * 14bit preamble (only 4 here to fit in 32bit)
 * 1 bit packet start bit (always 0)
 * 8 bit address (0-127, MSB always 0)
 * 1 bit address-data separator (always 0)
 * 8 bit data (different formats available)
 * 1 bit data-ecc separator (always 0)
 * 8 bit error checking code (address XOR data)
 * 1 bit packet end (always 1)
 *
 * @struct dcc_packet_t
 */
typedef unsigned int dcc_packet_t;

void dcc_setup (void);

/**
 * Actions of dcc_sender_t
 *
 * @defgroup dcc_t_actions dcc_t actions
 * @{
 */
void dcc_add_packet (dcc_packet_t packet);
void dcc_add_speed_packet (unsigned char address, int speed);
void dcc_add_data_packet (unsigned char address, unsigned char data);
void dcc_add_function_packet (unsigned char address, unsigned char function, unsigned char state);
/**@}*/


/**@}*/
#endif

/*
  Local variables:
    mode: c
    c-file-style: stroustrup
  End:
*/
