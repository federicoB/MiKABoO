#include "scheduler.h"



void scheduler(){
    //test one thread scheduling
    struct tcb_t * thread = sched_top(&readyQueue);
    LDST(&(thread->t_s));
    //TODO: implement the real sceduler
}

void sched_enqueue(struct list_head* sched_queue,struct tcb_t* thread){
    //enqueue adding the given thread to the tail of the list
    list_add_tail(&(thread->t_sched), sched_queue);
}

struct tcb_t* sched_dequeue(struct list_head* sched_queue){
    //get the top (or null if the queue is empty)
    struct tcb_t* thread = sched_top(sched_queue);
    //if not null
    if(thread != NULL){
        //delete the thread from the queue
        list_del(&(thread->t_sched));
    }
    //return the thread
    return thread;
}

struct tcb_t* sched_top(struct list_head* sched_queue){
    //the top of the queue
    tcb_t* result = NULL;
    //if the queue is not empty
    if(list_empty(sched_queue)){
        //get the top
        result = container_of(list_next(sched_queue),struct tcb_t, t_sched);
    }
    //return the result (or null)
    return result;
}
