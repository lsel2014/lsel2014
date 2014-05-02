#ifndef TIME_OPERATIONS
#define TIME_OPERATIONS
#include <time.h>
#include <sys/time.h>

void timeval_sub(struct timeval *res, struct timeval *a, struct timeval *b);

#endif
