#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "mikabooq.h"

//ready threads' queue
struct list_head readyQueue;

//waiting threads' queue
struct list_head waitQueue;

void scheduler();
void sched_enqueue(struct list_head *sched_queue, struct tcb_t *thread);
struct tcb_t* sched_dequeue(struct list_head* sched_queue);
struct tcb_t* sched_top(struct list_head* sched_queue);

#endif /* SCHEDULER_H */
