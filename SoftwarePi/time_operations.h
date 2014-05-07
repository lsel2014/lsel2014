/**
 * @file    time_operations.h
 * @date    --
 * @brief   Time functions
 *
 * Declaration of time managing functions
 */

/**
 * Time operations
 *
 * @defgroup time_operations
 * @{
 */

#ifndef TIME_OPERATIONS
#define TIME_OPERATIONS

#include <time.h>
#include <sys/time.h>

/**
 * Time managing functions
 *
 * @defgroup timeval_actions timeval actions
 * @{
 */
void timeval_sub(struct timeval *res, struct timeval *a, struct timeval *b);
/**@}*/


/**@}*/
#endif
