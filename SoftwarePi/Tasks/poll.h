#ifndef POLL_H
#define POLL_H

#define TASK_POLL_PERIOD 100000000

#include "sensorIR.h"

void daemon_poll_sensors(sensorIR_t *sensors);

#endif
