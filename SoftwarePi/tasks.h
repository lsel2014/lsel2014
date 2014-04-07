#ifndef TASKS_H
#define TASKS_H
#include <sys/mman.h>
#include <native/task.h>
#include <native/timer.h>
#include <stdlib.h>  
#define MAX_TASKS 10
#define PRIO_BASE 30
#define PRIO_STEP 5

typedef struct{
	RT_TASK* task;
	int prio;
	int deadline;
	void (*f)(void *);
	void* arg;
	char* name;
}taskinfo_t;

void task_add(char* name, int deadline, void (*f)(void *),void* arg);
void task_start_all();
void task_delete_all();
int task_cmp(const void* t1, const void* t2);
#endif