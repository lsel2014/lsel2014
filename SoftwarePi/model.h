#ifndef MODEL_H
#define MODEL_H

#include "observer.h"

void model_setup (void);
observable_t* model_get (const char* name);
observable_t* model_get_next (const char* name, int* i);
int model_add (const char* name, observable_t* obs);

#endif

/*
  Local variables:
    mode: c
    c-file-style: stroustrup
  End:
*/
