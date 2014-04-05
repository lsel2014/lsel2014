#ifndef FRAMES_GEN_H
#define FRAMES_GEN_H
#include <stdlib.h>
#include <pthread.h>

// DUMMY

typedef struct frames_t
{

	pthread_mutex_t mutex;

} frames_t;


//----------------------------

void frames_gen_init (frames_t* this);

frames_t* frames_gen_new (void);

#endif
