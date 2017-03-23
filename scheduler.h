#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "mikabooq.h"

//ready threads queue
struct list_head readyQueue;

//waiting threads queue
struct list_head waitQueue;

//running thread
struct tcb_t* runningThread;

//total number of threads
int totalThread;

//number of soft-blocked threads
int softBlockedThread;

void sched_init();
void scheduler();

#endif /* SCHEDULER_H */
