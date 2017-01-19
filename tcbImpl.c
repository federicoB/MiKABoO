#include "mikabooq.h"
#include "listx.h"
#include "const.h"

/* extracts a tcb from the free tcb list. Must check size first. */
struct tcb_t* extractTCB();

//the array of tcbs
struct tcb_t tcbArray[MAXTHREAD];
//tcb free list entry point
struct list_head freeTCB;

void thread_init(){
    //if invalid const exit
    if (MAXTHREAD <= 0) exit();
    //init the free list
    INIT_LIST_HEAD(&freeTCB);
    //for the whole array of tcbs
    for (int i = 0; i < MAXTHREAD; i++) {
        //add the tcb to the free list
        list_add(&(tcbArray[i].t_next), &freeTCB);
    }
}

struct tcb_t* thread_alloc(struct pcb_t* process){
    //the result
    struct tcb_t* newTCB = NULL;
    //if parent is not NULL and there is a free pcb
    if (process != NULL && !list_empty(&freeTCB)){
        //extract a tcb
        newTCB = extractTCB();
        //init message list in new pcb
        INIT_LIST_HEAD(&(newTCB->t_msgq));
        //insert the new tcb in the thread list of the process
        list_add(&(newTCB->t_next), &(process->p_threads));
        //TODO: choose how to init t_next and t_sched ---------------------
        //TODO: choose how toinit status ecc...
        //save the parent process
        newTCB->t_pcb = process;
    }//else leave as NULL
    //return the result
    return newTCB;
}

int thread_free(struct tcb_t* oldthread){
    //the result code (default: error)
    int result = -1;
    //if the thread has no pending message
    if(list_empty(&(oldthread->t_msgq))){
        //remove oldthread from the process's thread list
        list_del(oldthread->t_next);
        //add oldthread to the free list
        list_add(&(oldthread->t_next), &freeTCB);
        //set result as 0 (success)
        result = 0;
    }
    //return the result
    return result;
}


struct tcb_t* extractTCB(){
    //extract a tcb from the first list element
    struct tcb_t* extractedTCB = container_of(list_next(&freeTCB), tcb_t, t_next);
    //remove the extracted tcb from the free list
    list_del(&(extractedTCB->t_next));
    //return the tcb
    return extractedTCB;
}

void thread_enqueue(struct tcb_t *new, struct list_head *queue) {
    //append the given element to the list
    list_add_tail(&(new->t_sched),queue);
}

struct tcb_t *thread_qhead(struct list_head *queue) {
    //declare a variable for result value, default value is null
    struct tcb_t * result = NULL;
    //if the list is not empty
    if (!list_empty(queue)) {
        //get the first element of the queue
        result = container_of(list_next(queue),tcb_t,t_sched);
    }
    //return the thread
    return result;
}

struct tcb_t *thread_dequeue(struct list_head *queue) {
    //declare a variable for result value,default value is null
    struct tcb_t* result = NULL;
    //if the list is not empty
    if (!list_empty(queue)) {
        //get the first element of the queue
        struct list_head * first = list_next(queue);
        //set it as return value
        result = container_of(first,tcb_t,t_sched);
        //delete the first thread of the queue
        list_del(first);
    }
    //return the dequeued thread
    return result;
}
