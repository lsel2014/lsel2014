// Xenomai native skin libraries
#include <sys/mman.h>
#include <native/task.h>
#include <native/timer.h>

#include "poll.h"

#include "../Model/Sensors/sensorIR.h"

void daemon_poll_sensors(sensorIR_t *sensors) {
	/*
	 * Arguments: &task (NULL=self),
	 * start time,
	 * period (here: 1 s)
	 */
	rt_task_set_periodic(NULL, TM_NOW, TASK_POLL_PERIOD);

	while (1) {
		int i;
		rt_task_wait_period(NULL );
		rt_printf("Poll!\n");
		
		for (i=0;i<4;i++) {
			int read = sensorIR_trainPassing(sensors[i]);
			if (read >= 0)
				rt_printf("Detected train %i", read);
		}
	}
}
