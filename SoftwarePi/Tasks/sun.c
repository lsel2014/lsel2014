// Xenomai native skin libraries
#include <sys/mman.h>
#include <native/task.h>
#include <native/timer.h>

#include "sun.h"

void daemon_update_sun(void *arg) {
	/*
	 * Arguments: &task (NULL=self),
	 * start time,
	 * period (here: 1 s)
	 */
	rt_task_set_periodic(NULL, TM_NOW, TASK_SUN_PERIOD);

	while (1) {
		rt_task_wait_period(NULL );
		printf("Sun!\n");
	}
}
