#include "logger.h"
#include "model.h"
#include "train.h"
#include "Interpreter/interp.h"
#include <stdlib.h>

static int logger_cmd (char* arg);

void setup_logger(void)
{
   interp_addcmd("log", logger_cmd, "Print in a file.\n");
}

static int logger_cmd(char* arg)
{
   logger_new(arg);
   return 0;
}

logger_t* logger_new(const char* filename)
{
   logger_t* logger = (logger_t*) malloc(sizeof(logger_t));
   logger_init(logger, filename);
   return logger;
}

void logger_init(logger_t* logger, const char* filename)
{
   observer_init((observer_t*) logger, logger_notify);
   (*logger).file = fopen(filename, "a");
}

void logger_notify(observer_t* observer, observable_t* observable)
{
   logger_t* this = (logger_t*) observer;
   train_t* train = (train_t*) model_get_observable("Diesel");
   fprintf(this->file, "Tren %d en sector %d\n", train->ID, train->telemetry->sector);
}
