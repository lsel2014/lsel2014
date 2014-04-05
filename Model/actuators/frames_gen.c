#include "frames_gen.h"

// DUMMY

void frames_gen_init (frames_t* this){
	pthread_mutex_init(&this->mutex, NULL);
}

frames_t* frames_gen_new (void){
	frames_t* this = (frames_t*) malloc (sizeof (frames_t));
	frames_gen_init(this);

	return this;
}
