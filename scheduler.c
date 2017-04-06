#include <libuarm.h>
#include "scheduler.h"
#include "const.h"
#include "utils.h"


void scheduler(){
    //use unique TODLO
    unsigned int TODLO = TODLO_US();
    //handle pseudoclock
    unsigned int tickTimeLeft = handle_pseudoclock(TODLO);
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
                //set interval timer as next tick
                SET_IT_US(tickTimeLeft);
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
            //reset time slice
            timeSliceLeft = TIME_SLICE;
        }
    }
    //set interval timer as the minimum between time slice and next tick
    SET_IT_US(MIN(tickTimeLeft, timeSliceLeft));
    //set current time as last load time
    lastLoadTime = TODLO;
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
    //initialize last tick time to time of day (lower bits)
    lastTickTime = TODLO_US();
}

unsigned int handle_pseudoclock(unsigned int TODLO){
    //save time until next pseudoclock tick
    unsigned int tickTimeLeft = PSEUDO_TICK - (TODLO - lastTickTime);
    //if pseudoclock should tick
    if((TODLO - lastTickTime) >= PSEUDO_TICK){
        //increment last tick time
        lastTickTime += PSEUDO_TICK;
        //update tick time left
        tickTimeLeft = PSEUDO_TICK - (TODLO - lastTickTime);
        //TODO: handle tick behaviour
    }
    //return the time
    return tickTimeLeft;
}

void handle_accounting(){
    //compute last elapsed time
    unsigned int elapsed = TODLO_US() - lastLoadTime;
    //charge the process elapsed time
    runningThread->t_pcb->CPU_time += elapsed;
    //if time slice execeeded (cast in order to avoid overflow)
    if((long int) timeSliceLeft - (long int) elapsed <= 0){
        //move thread to readyQueue
        thread_enqueue(runningThread, &readyQueue);
        //set running thread as NULL
        runningThread = NULL;
    }
    else {
        //decrease time slice left
        timeSliceLeft -= elapsed;
    }
}