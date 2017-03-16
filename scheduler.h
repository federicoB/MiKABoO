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
void sched_enqueue(struct list_head *sched_queue, struct tcb_t *thread);
struct tcb_t* sched_dequeue(struct list_head* sched_queue);
struct tcb_t* sched_top(struct list_head* sched_queue);

#endif /* SCHEDULER_H */
