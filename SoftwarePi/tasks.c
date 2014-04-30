#include <sys/mman.h>
#include <native/task.h>
#include <native/timer.h>
#include "tasks.h"
#include "Interpreter/interp.h"

taskinfo_t* tasks[MAX_TASKS];
int numtasks = 0;

/**
 * task_cmd
 *
 * Interpreter command used to get information from all running tasks
 *
 * @param arg subcmd string
 * @return 0 if subcmd exists, 1 otherwise
 */
int task_cmd(char* arg) {
	if (0 == strcmp(arg, "list")) {
		printf("NAME\t\tDEADLINE(ns)\t\tPRIORITY\n");
		int i;
		for (i = 0; i < numtasks; ++i) {
			printf("%s\t\t%d\t\t%d\r\n", tasks[i]->name, tasks[i]->deadline,
					tasks[i]->prio);
		}
		return 0;
	}
	return 1;
}

/**
 * task_add
 *
 * Adds a new task to the task manager if there's room for it
 *
 * @param name the name of the task
 * @param deadline the task deadline in nanoseconds
 * @param f the function associated to the task
 * @param arg the argument for the task function
 *
 */
void task_add(char* name, int deadline, void (*f)(void *), void* arg) {
	if (numtasks < MAX_TASKS) {
		RT_TASK* new_task = malloc(sizeof(RT_TASK));
		taskinfo_t* this = malloc(sizeof(taskinfo_t));
		this->name = name;
		this->task = new_task;
		this->deadline = deadline;
		this->f = f;
		this->arg = arg;
		tasks[numtasks++] = this;
	}
}

/**
 * task_start_all
 *
 * Sorts the available real-time tasks by deadline and assigns them
 * priorities that are deadline-monotonic. Then launches all the tasks
 *
 */

void task_start_all() {
	qsort(tasks, numtasks, sizeof(taskinfo_t*), task_cmp); //sort by deadline
	int i;
	int prio = PRIO_BASE;
	for (i = 0; i < numtasks; i++, prio += PRIO_STEP) {
		tasks[i]->prio = prio;
		rt_task_create(tasks[i]->task, tasks[i]->name, 0, prio, 0);
		rt_task_start(tasks[i]->task, tasks[i]->f, tasks[i]->arg);
	}
	interp_addcmd("task", task_cmd, "Task information");
}

/**
 * task_cmp
 *
 * Comparison function used by qsort to sort tasks by
 * deadline
 *
 * @param t1 one task
 * @param t2 another task
 * @return -1,0 or 1 according to qsort specifications
 */

int task_cmp(const void* t1, const void* t2) {
	taskinfo_t* task1 = (taskinfo_t*) t1;
	taskinfo_t* task2 = (taskinfo_t*) t2;
	if (task1->deadline < task2->deadline)
		return -1;
	if (task1->deadline == task2->deadline)
		return 0;
	return 1;
}

/**
 * task_delete_all
 *
 * Stops all running tasks.
 *
 */

void task_delete_all() {
	int i;
	for (i = 0; i < numtasks; i++) {
		rt_task_delete(tasks[i]->task);
	}
}
;

