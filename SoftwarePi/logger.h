#ifndef LOGGER_H
#define LOGGER_H

#include "../observer.h"
#include <stdio.h>

typedef struct {
   observer_t observer;
   FILE* file;
} logger_t;

void setup_logger(void);
int logger_cmd(char* arg);
logger_t* logger_new(const char* filename);
void logger_init(logger_t* logger, const char* filename);
void logger_notify(observer_t* observer, observable_t* observable);

#endif
