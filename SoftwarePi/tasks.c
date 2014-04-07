#include <sys/mman.h>
#include <native/task.h>
#include <native/timer.h>

taskinfo_t* tasks[MAX_TASKS];
int numasks=0;

void task_add(char* name, int deadline, void (*f)(void *),void* arg){
	RT_TASK* new_task = malloc(sizeof(RT_TASK));
	taskinfo_t* this = malloc(sizeof(taskinfo_t));
	this->name= name;
	this->task=new_task;
	this->deadline = deadline;
	this->f =f;
	this->arg =arg;
	tasks[numtasks++]=this;
}

void task_start_all(){
	qsort(tasks,numtasks,sizeof(taskinfo_t*),task_cmp); //sort by deadline
	int i;
	int prio=PRIO_BASE;
	for(i=0;i<numtasks;i++,prio+=PRIO_STEP){
		rt_task_create(tasks[i]->task,tasks[i]->name,0,prio,0);
		rt_task_start(tasks[i]->task,tasks[i]->f,tasks[i]->arg);
	}
}

void task_delete_all(){
	int i;
	for(i=0;i<numtasks;i++){
		rt_task_delete(tasks[i]->task);
	}
};

int task_cmp(const void* t1, const void* t2){
	if((taskinfo_t*)t1->deadline < (taskinfo_t*)t2->deadline) return 1; 
	if((taskinfo_t*)t1->deadline == (taskinfo_t*)t2->deadline) return 0; 
	return -1;
}