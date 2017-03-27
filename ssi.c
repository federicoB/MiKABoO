#include "ssi.h"
#include "const.h"

void SSI_entry_point(){
    //TODO: implement
    
    //forever (until killed)
    while(1){
        //receive a message
        //handle service request
        //reply to the message (if necessary)
    }
}

void proc_terminate(struct pcb_t* proc){
    //declare an iterator
    struct pcb_t* children;
    //terminate the children of the process
    list_for_each_entry(children, &(proc->p_children), p_siblings){
        //DFS-style recursive call
        proc_terminate(children);
    }
    //the process has no childern at this point
    //declare an iterator
    struct tcb_t* thread;
    //delete threads of the process
    list_for_each_entry(thread, &(proc->p_threads), t_next){
        //terminate thread
        thread_terminate(thread);
    }
    //the process hasn't any threads or children
    //delete the process
    proc_delete(proc);
}

struct pcb_t *proc_create(struct pcb_t *parent, state_t *state) {
    //declare a pointer to the new process
    struct pcb_t *process;
    //allocate a new process if possible
    if ((process = proc_alloc(parent)) != NULL) {
        //declare a pointer to the new thread
        struct tcb_t *firstThread;
        //allocate a new thread for the new process if possible
        if ((firstThread = thread_alloc(process)) != NULL) {
            //TODO copy the state into the new thread (need the implementation of memcopy)
        }
        //if some error occurred during thread allocation
        else {
            //set the process address to an error code
            process = (struct pcb_t *) CREATENOGOOD;
        }
    }
    //if some error occurred during process allocation
    else {
        //set the process addreass to an error code
        process = (struct pcb_t *) CREATENOGOOD;
    }
    //return the new process
    return process;
}

void thread_terminate(struct tcb_t* thread){
    //TODO:check which queue contains the thread.
    //if in wait queue
        //decrease softblock
        //softBlockedThread--
        //update wait structures if necessary
    //else if in ready queue -> do nothing special
    
    //sender of the message
    struct tcb_t* sender = NULL;
    //the message payload
    uintptr_t payload;
    //while there are pending messages
    while((msgq_get(&sender, thread, &payload)) != -1){
        //TODO: discard the message (send discard reply)
        
        //reset the sender as NULL
        sender = NULL;
    }

    //remove the thread from the queue (no matter which one)
    list_del(&(thread->t_sched));
    //decrease the number of threads
    totalThread--;
    //free the thread
    thread_free(thread);
}

struct tcb_t* thread_create(struct pcb_t* process, state_t* state){
    //the created thread
    struct tcb_t* thread = NULL;
    //if the allocation succedes (process != NULL && at least 1 free thread)
    if((thread = thread_alloc(process)) != NULL){
        //TODO: copy the state
        //enqueue the thread in the ready queue
        thread_enqueue(thread, &readyQueue);
        //increase the total number of threads
        totalThread++;
    }
    //else if fails
    else{
        //set the thread address as CREATENOGOOD
        thread = (struct tcb_t*) CREATENOGOOD;
    }
    //return the result
    return thread;
}