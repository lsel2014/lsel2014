
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/mman.h>
#include <native/task.h>
#include <native/timer.h>
/* NOTE: error handling omitted. */
void daemon_poll_sensors(void *arg) {
	/*
	 * Arguments: &task (NULL=self),
	 * start time,
	 * period (here: 1 s)
	 */
	rt_task_set_periodic(NULL, TM_NOW, 100000000);
	while (1) {
		rt_task_wait_period(NULL);
		printf ("Poll!\n");
	}
}
void daemon_send_trainpackets(void *arg) {
	/*
	 * Arguments: &task (NULL=self),
	 * start time,
	 * period (here: 1 s)
	 */
	rt_task_set_periodic(NULL, TM_NOW, 500000000);
	while (1) {
		rt_task_wait_period(NULL);
		printf("Packet!\n");
	}
}

void daemon_update_sun(void *arg) {
	/*
	 * Arguments: &task (NULL=self),
	 * start time,
	 * period (here: 1 s)
	 */
	rt_task_set_periodic(NULL, TM_NOW, 2000000000);
	while (1) {
		rt_task_wait_period(NULL);
		printf ("Sun!\n");
	}
}

void catch_signal(int sig) {
}
int main(int argc, char* argv[]) {
	RT_TASK task_poll, task_packets, task_sun;


	signal(SIGTERM, catch_signal);
	signal(SIGINT, catch_signal);
	/* Avoids memory swapping for this program */
	mlockall(MCL_CURRENT | MCL_FUTURE);
	/*
	 * Arguments: &task,
	 * name,
	 * stack size (0=default),
	 * priority,
	 * mode (FPU, start suspended, ...)
	 */
	rt_task_create(&task_poll, "polling", 0, 50, 0);
	rt_task_create(&task_packets, "packets", 0, 55, 0);
	rt_task_create(&task_sun, "sun", 0, 20, 0);
	/*
	 * Arguments: &task,
	 * task function,
	 * function argument*/
	rt_task_start(&task_poll, &daemon_poll_sensors, NULL);
	rt_task_start(&task_packets, &daemon_send_trainpackets, NULL);
	rt_task_start(&task_sun, &daemon_update_sun, NULL);

	pause();
	rt_task_delete(&task_poll);
	rt_task_delete(&task_packets);
	rt_task_delete(&task_sun);
	return 0;
}
