#ifndef MODEL_H
#define MODEL_H

void model_init (void);
observable_t* model_get (const char* name);
int model_add (const char* name, observable_t* obs);

#endif
