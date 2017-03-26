#include "ssi.h"

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