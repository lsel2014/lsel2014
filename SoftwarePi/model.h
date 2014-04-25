#ifndef MODEL_H
#define MODEL_H

#include "Model/observer.h"
#define MAXIRSENSORS 4
#define MAXTRAINS 8

void model_init (void);
observable_t* model_get_IRsensor (const char* name);
observable_t* model_get_train (const char* name);
int model_add (const char* name, observable_t* obs);

#endif
