#include "ssi.h"
#include "const.h"
#include "utils.h"
#include "uARMconst.h"

void SSI_entry_point(){
    //TODO: implement
    
    //forever (until killed)
    while(1){
        //pointer to the ssi msg
        struct ssimsg_t* msg = NULL;
        //receive a message and save the sender
        struct tcb_t* thread = MsgRecv(NULL, &msg);
        //TODO: handle service request
        //TODO: reply to the message (if necessary)
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

struct pcb_t* proc_create(struct pcb_t* parent, state_t* state) {
    //declare a pointer to the new process
    struct pcb_t* process;
    //allocate a new process and if succeeded
    if ((process = proc_alloc(parent)) != NULL) {
        //allocate a new thread for the new process and if failed
        if (thread_create(process, state) == (struct tcb_t*) CREATENOGOOD) {
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
    //if thread to delete is running
    if(thread == runningThread){
        //set no running thread
        runningThread = NULL;
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

unsigned int get_CPU_time(struct pcb_t* applicant){
    //return the CPU time of the applicant's parent process
    return applicant->CPU_time;
}

int set_SYSBK_manager(struct tcb_t* applicant, struct tcb_t* manager){
    //the return code (default: error)
    int result = -1;
    //if the managers already set
    if (applicant->t_pcb->sysMgr != NULL){
        //terminate the process
        proc_terminate(applicant->t_pcb);
    }
    //otherwise if not set
    else{
        //set the manager
        applicant->t_pcb->sysMgr = manager;
        //set the result as succeeded
        result = 0;
    }
    //return the result
    return result;
}

int set_TLB_manager(struct tcb_t* applicant, struct tcb_t* manager){
    //the return code (default: error)
    int result = -1;
    //if the managers already set
    if (applicant->t_pcb->tlbMgr != NULL){
        //terminate the process
        proc_terminate(applicant->t_pcb);
    }
    //otherwise if not set
    else{
        //set the manager
        applicant->t_pcb->tlbMgr = manager;
        //set the result as succeeded
        result = 0;
    }
    //return the result
    return result;
}

int set_PGM_manager(struct tcb_t* applicant, struct tcb_t* manager){
    //the return code (default: error)
    int result = -1;
    //if the managers already set
    if (applicant->t_pcb->pgmMgr != NULL){
        //terminate the process
        proc_terminate(applicant->t_pcb);
    }
    //otherwise if not set
    else{
        //set the manager
        applicant->t_pcb->pgmMgr = manager;
        //set the result as succeeded
        result = 0;
    }
    //return the result
    return result;
}

void SSIRequest(unsigned int service, unsigned int payload, unsigned int* reply){
    //TODO: implement
    
    //declare a "fat" message
    struct ssimsg_t ssimsg;
    //set the service
    ssimsg.service = service;
    //set the argument
    ssimsg.argument = payload;
    //send a message to the SSI
    MsgSend(SSI_addr, &ssimsg);
    //receive a message from the SSI
    MsgRecv(SSI_addr, reply);
}