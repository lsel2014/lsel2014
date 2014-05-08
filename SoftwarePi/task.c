#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include "task.h"
#include "utils.h"
#include "interp.h"

#define MAXTASKS 20

typedef struct taskinfo_t {
  pthread_t tid;
  const char* name;
  struct timeval period;
  struct timeval deadline;
  int prio;
  struct timeval maxtime;
} taskinfo_t;

static taskinfo_t task[MAXTASKS];
static int ntasks = 0;

static
taskinfo_t*
taskinfo_find (pthread_t tid)
{
  int i;
  for (i = 0; i < ntasks; ++i)
    if (tid == task[i].tid)
      return &task[i];
  return NULL;
}

static int
cmd_task (char* arg)
{
  if (0 == strcmp (arg, "list")) {
    int i;
    printf ("Id\tName           \tT\tD\tP\tR\n");
    for (i = 0; i < ntasks; ++i) {
      printf ("%d\t%-15s\t%d\t%d\t%d\t%d\n", i, task[i].name,
              timeval_get_ms(&task[i].period),
              timeval_get_ms(&task[i].deadline),
              task[i].prio,
              timeval_get_ms(&task[i].maxtime));
    }
    return 0;
  }
  return 1;
}

void
task_setup (void)
{
  interp_addcmd ("task", cmd_task, "task list");
}

void
task_register_time (pthread_t tid, struct timeval* time)
{
  taskinfo_t* this = taskinfo_find (tid);
  if (timeval_less (&this->maxtime, time)) {
    this->maxtime.tv_sec = time->tv_sec;
    this->maxtime.tv_usec = time->tv_usec;
  }
}

struct timeval*
task_get_period (pthread_t tid)
{
  taskinfo_t* this = taskinfo_find (tid);
  return &this->period;
}

struct timeval*
task_get_deadline (pthread_t tid)
{
  taskinfo_t* this = taskinfo_find (tid);
  return &this->deadline;
}

pthread_t
task_new (const char* name, void *(*f)(void *),
          int period_ms, int deadline_ms, int stacksize)
{
  taskinfo_t* t = &task[ntasks++];
  pthread_attr_t attr;
  struct sched_param sparam;
  sparam.sched_priority = sched_get_priority_min (SCHED_FIFO);

  pthread_attr_init (&attr);
  pthread_attr_setstacksize (&attr, stacksize);
  pthread_attr_setscope (&attr, PTHREAD_SCOPE_SYSTEM);
  pthread_attr_setschedpolicy (&attr, SCHED_FIFO);
  pthread_attr_setschedparam (&attr, &sparam);
  pthread_create (&t->tid, &attr, f, t);

  t->name = name;
  t->prio = 0;
  t->period.tv_sec = period_ms / 1000;
  t->period.tv_usec = (period_ms % 1000) * 1000;
  t->deadline.tv_sec = deadline_ms / 1000;
  t->deadline.tv_usec = (deadline_ms % 1000) * 1000;
  t->maxtime.tv_sec = t->maxtime.tv_usec = 0;

  return t->tid;
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

static
int
task_cmp (const void* t1, const void* t2)
{
  taskinfo_t* task1 = (taskinfo_t*) t1;
  taskinfo_t* task2 = (taskinfo_t*) t2;
  if (timeval_less (&task1->deadline, &task2->deadline))
    return -1;
  if (timeval_equal (&task1->deadline, &task2->deadline))
    return 0;
  return 1;
}

/**
 * task_setup_priorities
 *
 * Sorts the available real-time tasks by deadline and assigns them
 * priorities that are deadline-monotonic. Then launches all the tasks
 *
 */

void
task_setup_priorities (void)
{
  int i;
  qsort (task, ntasks, sizeof(taskinfo_t*), task_cmp);
  for (i = 0; i < ntasks; ++i) {
    int policy;
    struct sched_param sparam;
    pthread_getschedparam (task[i].tid, &policy, &sparam);
    sparam.sched_priority = sched_get_priority_min (SCHED_FIFO) + i;
    task[i].prio = i;
    pthread_setschedparam (task[i].tid, policy, &sparam);
  }
}


/**
 * task_delete_all
 *
 * Stops all running tasks.
 *
 */

void
task_delete_all (void)
{
  while (ntasks) {
    pthread_cancel (task[ntasks - 1].tid);
    ntasks--;
  }
}


void
mutex_init (pthread_mutex_t* m)
{
  pthread_mutexattr_t attr;
  pthread_mutexattr_init (&attr);
  pthread_mutexattr_setprotocol (&attr, PTHREAD_PRIO_INHERIT);
  pthread_mutex_init (m, &attr);
}



/*
  Local variables:
    c-file-style: stroustrup
  End:
*/
