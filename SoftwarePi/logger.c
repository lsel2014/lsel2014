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

static int logger_cmd (char* arg)
{
   char* obs = strchr (arg, ' ');
   if (obs == NULL)
      return 1;
   *obs++ = '\0';
   logger_new (arg, obs);
   return 0;
}

logger_t* logger_new(const char* filename, const char* observable)
{
   logger_t* logger = (logger_t*) malloc(sizeof(logger_t));
   logger_init(logger, filename, observable);
   return logger;
}

void logger_init(logger_t* logger, const char* filename, const char* observable)
{
   observer_init((observer_t*) logger, logger_notify);
   logger->filename = strdup (filename);
   logger->observable = model_get_observable (observable);
}

void logger_notify(observer_t* observer, observable_t* observable)
{
   logger_t* this = (logger_t*) observer;
   FILE* f = fopen(this->filename, "a");
   // print timestamp
   observable_dump (this->observable, f);
   fclose (f);
}
