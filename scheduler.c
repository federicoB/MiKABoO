#include "scheduler.h"



void scheduler(){
	//test one thread scheduling
    struct tcb_t * thread = top(&readyQueue);
    LDST(&(thread->t_s));
	//TODO: implement the real sceduler
}

void sched_enqueue(struct list_head* sched_queue,struct tcb_t* thread){
    list_add_tail(&(thread->t_sched), sched_queue);
}

struct tcb_t* sched_dequeue(struct list_head* sched_queue){
	struct tcb_t* thread = sched_top(sched_queue);
	//check for null
	if(thread != NULL){
	    //delete the message from the queue
	    list_del(&(thread->t_sched));
	}
    return thread;
}

struct tcb_t* sched_top(struct list_head* sched_queue){
	return container_of(list_next(sched_queue),struct tcb_t, t_sched);
}
