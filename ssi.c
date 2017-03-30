#include "ssi.h"
#include "const.h"
#include "utils.h"
#include "uARMconst.h"

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
    struct pcb_t* child;
    //terminate the children of the process
    while((child = proc_firstchild(proc)) != NULL){
        //DFS-style recursive call
        proc_terminate(child);
    }
    //the process has no childern at this point
    //declare an iterator
    struct tcb_t* thread;
    //delete threads of the process
    while((thread = proc_firstthread(proc)) != NULL){
        //terminate thread
        thread_terminate(thread);
    }
    //the process hasn't any threads or children
    //delete the process
    proc_delete(proc);
}

struct pcb_t *proc_create(struct pcb_t *parent, state_t *state) {
    //declare a pointer to the new process
    struct pcb_t* process;
    //allocate a new process and if succeeded
    if ((process = proc_alloc(parent)) != NULL) {
        //allocate a new thread for the new process and if failed
        if (thread_create(process, state) == (struct tcb_t *) CREATENOGOOD) {
            //delete the process
            proc_delete(process);
            //set the process address to an error code
            process = (struct pcb_t*) CREATENOGOOD;
        }
    }
    //else if process alllocation failed
    else {
        //set the process addreass to an error code
        process = (struct pcb_t*) CREATENOGOOD;
    }
    //return the new process
    return process;
}

void thread_terminate(struct tcb_t* thread){
    //if in wait queue (not in ready and not unused)
    if(thread->t_status != T_STATUS_READY){
        //decrease softblock
        softBlockedThread--;
        //TODO: update wait structures if necessary (e.g.: IO)
    }
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
    //TODO: handle messages with this thread as a sender
    //TODO: handle threads waiting for messages from this thread
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
        //copy the state
        memcopy(state, &(thread->t_s), sizeof(state_t));
        //enqueue the thread in the ready queue
        thread_enqueue(thread, &readyQueue);
        //set status as ready
        thread->t_status = T_STATUS_READY;
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