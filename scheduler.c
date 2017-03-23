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
            runningThread = thread_dequeue(&readyQueue);
        }
    }
    //load the running thread's state
    LDST(&(runningThread->t_s));
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