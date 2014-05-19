#ifndef LOGGER_H
#define LOGGER_H

#include "Model/observer.h"

typedef struct {
   observer_t observer;
   const char* filename;
   observable_t* observable;
} logger_t;

void setup_logger(void);
logger_t* logger_new(const char* filename, const char* observable);
void logger_init(logger_t* logger, const char* filename, const char* observable);
void logger_notify(observer_t* observer, observable_t* observable);

#endif
