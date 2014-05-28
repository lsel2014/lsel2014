#ifndef DAEMON_H
#define DAEMON_H

// Task priorities
#include <native/mutex.h>

#define TASK_DCC_PRIORITY 40
#define TASK_POLL_PRIORITY 30
#define TASK_SUN_PRIORITY 20
#define TASK_INTERPRETER_PRIORITY 10

typedef struct{
        int i2chandler;
        RT_MUTEX mutex;
}i2chandler_t;

extern i2chandler_t* i2c0handle,i2c1handle;

#endif
