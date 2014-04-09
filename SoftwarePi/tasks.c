#include <sys/mman.h>
#include <native/task.h>
#include <native/timer.h>
#include "tasks.h"
#include "Interpreter/interp.h"

taskinfo_t* tasks[MAX_TASKS];
int numtasks = 0;

int task_cmd(char* arg){
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

void task_add(char* name, int deadline, void (*f)(void *), void* arg) {
	RT_TASK* new_task = malloc(sizeof(RT_TASK));
	taskinfo_t* this = malloc(sizeof(taskinfo_t));
	this->name = name;
	this->task = new_task;
	this->deadline = deadline;
	this->f = f;
	this->arg = arg;
	tasks[numtasks++] = this;
}

void task_start_all() {
	qsort(tasks, numtasks, sizeof(taskinfo_t*), task_cmp); //sort by deadline
	int i;
	int prio = PRIO_BASE;
	for (i = 0; i < numtasks; i++, prio += PRIO_STEP) {
		tasks[i]->prio = prio;
		rt_task_create(tasks[i]->task, tasks[i]->name, 0, prio, 0);
		rt_task_start(tasks[i]->task, tasks[i]->f, tasks[i]->arg);
	}
	interp_addcmd("task",task_cmd,"Task information");
}

void task_delete_all() {
	int i;
	for (i = 0; i < numtasks; i++) {
		rt_task_delete(tasks[i]->task);
	}
}
;

int task_cmp(const void* t1, const void* t2) {
	taskinfo_t* task1 = (taskinfo_t*) t1;
	taskinfo_t* task2 = (taskinfo_t*) t2;
	if (task1->deadline < task2->deadline)
		return -1;
	if (task1->deadline == task2->deadline)
		return 0;
	return 1;
}
