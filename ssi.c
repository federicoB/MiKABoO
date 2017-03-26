#include "ssi.h"
#include "mikabooq.h"

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
        thread_free(thread);
    }
    //the process hasn't any threads or children
    //delete the process
    proc_delete(proc);
}