#include <libuarm.h>
#include "scheduler.h"



void scheduler(){
    
    //TODO: implement interval logic.
    //TODO: set interval timer.
    
    //if there is no thread running
    if(runningThread == NULL){
        //if the ready queue is empty
        if(list_empty(&readyQueue)){
            //if there is no thread
            if(totalThread == 0){
                //halt the system
                HALT();
            }
            //else if there are threads but they are neither ready nor soft-blocked
            else if(totalThread > 0 && softBlockedThread == 0){
                //a deadlock happened, don't panic (pun intended) 
                PANIC();
            }
            //else if there are threads and some of them are soft-blocked
            else if(totalThread > 0 && softBlockedThread > 0){
                //unmask all interrupts
                setSTATUS(STATUS_ALL_INT_ENABLE(getSTATUS()));
                //set the CPU in WAIT mode
                WAIT();
            }
        }
        //else (non empty queue)
        else {
            //dequeue a thread and set it as running
            runningThread = sched_dequeue(&readyQueue);
        }
    }
    //load the running thread's state
    LDST(&(runningThread->t_s));
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
    struct tcb_t* result = NULL;
    //if the queue is not empty
    if(!list_empty(sched_queue)){
        //get the top
        result = container_of(list_next(sched_queue), struct tcb_t, t_sched);
    }
    //return the result (or null)
    return result;
}

void sched_init(){
    //initialize threads ready queue
    INIT_LIST_HEAD(&readyQueue);
    //initialize threads wait queue
    INIT_LIST_HEAD(&waitQueue);
    //initialize the running thread pointer as NULL
    runningThread = NULL;
    //initialize the number of threads to zero
    totalThread = 0;
    //initialize the number of soft-blocked threads to zero
    softBlockedThread = 0;
}