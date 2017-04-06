#include "exception.h"
#include "scheduler.h"
#include "utils.h"
#include "uARMconst.h"
#include "ssi.h"

void tlb_handler(){
    //TODO: implement
    //TODO: accounting if necessary
    
    //copy the old state in the tcb
    memcopy((state_t*) TLB_OLDAREA, &(runningThread->t_s), sizeof(state_t));
    //if the process (owner of the running thread) has a TLB trap manager set
    if(runningThread->t_pcb->tlbMgr != NULL){
        //pass up the exception
        trap_passup(runningThread->t_pcb->tlbMgr);
    }
    //else (unrecoverable exception)
    else{
        //terminate the process, all its threads, and the whole subtree
        proc_terminate(runningThread->t_pcb);
    }
    //handle thread accounting (at the end of the handler)
    handle_accounting();
    //call the scheduler
    scheduler();
}

void pgm_trap_handler(){
    //TODO: implement
    //TODO: accounting if necessary
    
    //copy the old state in the tcb
    memcopy((state_t*) PGMTRAP_OLDAREA, &(runningThread->t_s), sizeof(state_t));
    //if the process (owner of the running thread) has a PGM trap manager set
    if(runningThread->t_pcb->pgmMgr != NULL){
        //pass up the exception
        trap_passup(runningThread->t_pcb->pgmMgr);
    }
    //else (unrecoverable exception)
    else{
        //terminate the process, all its threads and the whole subtree
        proc_terminate(runningThread->t_pcb);
    }
    //handle thread accounting (at the end of the handler)
    handle_accounting();
    //call the scheduler
    scheduler();
}

void sys_bk_handler(){
    //TODO: implement
    
    //copy the old state in the tcb
    memcopy((state_t*) SYSBK_OLDAREA, &(runningThread->t_s), sizeof(state_t));
    
    //get the cause
    unsigned int cause = CAUSE_EXCCODE_GET(runningThread->t_s.CP15_Cause);
    //if the exception is a syscall
    if(cause == EXC_SYSCALL){
        //TODO: handle
    }
    //else if the exception is a breakpoint
    else if (cause == EXC_BREAKPOINT){
        //TODO: handle
    }
    //else if something went wrong
    else{
        //print an error (here interrupts are masked)
        tprint("SYS/BK HANDLER: Something went wrong\n");
        //panic
        PANIC();
    }
    //handle thread accounting (at the end of the handler)
    handle_accounting();
    //call the scheduler
    scheduler();
}

void trap_passup(struct tcb_t* manager){
    //send a message to the manager with the cause as payload
    //and the offending thread as sender. Save the return value.
    int succeeded = msgq_add(runningThread, manager, runningThread->t_s.CP15_Cause);
    //if send succeeded
    if(succeeded == 0){
        //enqueue in wait queue
        thread_enqueue(runningThread, &waitQueue);
        //increase soft blocked threads number
        softBlockedThread++;

        //TODO: set status as wait for trap

        //remove the thread from running
        runningThread = NULL;
        //if the manager is not in ready queue
        if(manager->t_status != T_STATUS_READY){
            //TODO: wake manager up
        }
    }
    //else (message limit exceeded)
    else{
        //print error message and panic
        tprint("CAN'T PASS UP TRAP: send failed\n");
        PANIC();
    }
}