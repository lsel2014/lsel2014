/**
 * @file    time_operations.c
 * @date    --
 * @brief   Time functions definition
 *
 * Implementation of time managing functions
 */

#include <time.h>
#include <sys/time.h>

/**
 * @brief timeval difference calculation
 *
 * Returns the difference in time between two
 * timeval structures.
 *
 * @ingroup time_operations
 *
 * @param *res      Result of the operation
 * @param *a        First operand
 * @param *b        Second operand
 */
void
timeval_sub (struct timeval *res, struct timeval *a, struct timeval *b)
{
  res->tv_sec = a->tv_sec - b->tv_sec;
  res->tv_usec = a->tv_usec - b->tv_usec;
  if (res->tv_usec < 0) {
    --res->tv_sec;
    res->tv_usec += 1000000;
  }
}

void
timeval_add (struct timeval *res, struct timeval *a, struct timeval *b)
{
  res->tv_sec = a->tv_sec + b->tv_sec
    + a->tv_usec / 1000000 + b->tv_usec / 1000000; 
  res->tv_usec = a->tv_usec % 1000000 + b->tv_usec % 1000000;
}

int
timeval_less (struct timeval *a, struct timeval *b)
{
  return (a->tv_sec < b->tv_sec) ||
    ((a->tv_sec == b->tv_sec) && (a->tv_usec < b->tv_usec));
}

int
timeval_equal (struct timeval *a, struct timeval *b)
{
  return (a->tv_sec == b->tv_sec) && (a->tv_usec == b->tv_usec);
}

int
timeval_get_ms (struct timeval *a)
{
  return a->tv_sec * 1000 + a->tv_usec / 1000;
}


/*
  Local variables:
    mode: c
    c-file-style: stroustrup
  End:
*/
