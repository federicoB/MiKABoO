#include <libuarm.h>
#include "exception.h"
#include "scheduler.h"
#include "utils.h"
#include "uARMconst.h"
#include "ssi.h"
#include "const.h"
#include "mikabooq.h"
#include <stdint.h>

void tlb_handler(){
    //TODO: implement
    //TODO: accounting if necessary
    
    //copy the old state in the tcb
    memcopy((state_t*) TLB_OLDAREA, &(runningThread->t_s), sizeof(state_t));
    //decrease the program counter by a word size. (due to the Link Return register behaviour)
    runningThread->t_s.pc -= WORD_SIZE;
    //if the process (owner of the running thread) has a TLB trap manager set
    if(runningThread->t_pcb->tlbMgr != NULL){
        //save thread before passup
        struct tcb_t* thread = runningThread;
        //pass up the exception
        trap_passup(runningThread->t_pcb->tlbMgr);
        //handle thread accounting (at the end of the handler)
        handle_accounting(thread);
    }
    //else (unrecoverable exception)
    else{
        //terminate the process, all its threads, and the whole subtree
        proc_terminate(runningThread->t_pcb);
    }
    //call the scheduler
    scheduler();
}

void pgm_trap_handler(){
    //TODO: implement
    //TODO: accounting if necessary
    
    //copy the old state in the tcb
    memcopy((state_t*) PGMTRAP_OLDAREA, &(runningThread->t_s), sizeof(state_t));
    //decrease the program counter by a word size. (due to the Link Return register behaviour)
    runningThread->t_s.pc -= WORD_SIZE;
    //if the process (owner of the running thread) has a PGM trap manager set
    if(runningThread->t_pcb->pgmMgr != NULL){
        //save thread before passup
        struct tcb_t* thread = runningThread;
        //pass up the exception
        trap_passup(runningThread->t_pcb->pgmMgr);
        //handle thread accounting (at the end of the handler)
        handle_accounting(thread);
    }
    //else (unrecoverable exception)
    else{
        //terminate the process, all its threads and the whole subtree
        proc_terminate(runningThread->t_pcb);
    }
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
        //get system call number
        unsigned int sysCallNumber = runningThread->t_s.a1;
        //if this is a recognized sys call
        if(sysCallNumber == SEND || sysCallNumber == RECV){
            //if in kernel mode
            if((runningThread->t_s.cpsr & STATUS_SYS_MODE) == STATUS_SYS_MODE){
                //save the running thread
                struct tcb_t* thread = runningThread;
                //if the system call is a send
                if(sysCallNumber == SEND){
                    //get the destination
                    struct tcb_t* dst = (struct tcb_t*)(runningThread->t_s.a2);
                    //get the payload
                    uintptr_t payload = (uintptr_t)(runningThread->t_s.a3);
                    //if this is a reply from a manager
                    if(dst->t_pcb->pgmMgr == runningThread ||
                        dst->t_pcb->tlbMgr == runningThread ||
                        dst->t_pcb->sysMgr == runningThread){
                        //remove dst from the wait queue
                        list_del(&(dst->t_sched));
                        //decrease soft blocked threads number
                        softBlockedThread--;
                        //move thread to readyQueue
                        thread_enqueue(dst, &readyQueue);
                        //set dst status to ready
                        dst->t_status = T_STATUS_READY;    
                    }
                    //else if a normal message
                    else{
                        //send the message (and wake the thread if necessary)
                        do_send(runningThread, dst, payload);
                    }
                }
                //if the system call is a receive
                else{
                    //get expected sender (or NULL if any message)
                    struct tcb_t* sender = (struct tcb_t*)(runningThread->t_s.a2);
                    //get the payload pointer
                    uintptr_t* payloadP = (uintptr_t*)(runningThread->t_s.a3);
                    //get the message from the queue and save the result
                    int succeeded = msgq_get(&sender, runningThread, payloadP);
                    //if succeeded
                    if(succeeded == 0){
                        //save sender in a0
                        runningThread->t_s.a1 = (unsigned int)sender;
                    }
                    //otherwise
                    else{
                        //decrement stack pointer so recv will be called again.
                        runningThread->t_s.pc -= WORD_SIZE;
                        //increment soft blocked threads number
                        softBlockedThread++;
                        //move thread to waitQueue
                        thread_enqueue(runningThread, &waitQueue);
                        //set thread status to wait for message
                        runningThread->t_status = T_STATUS_W4MSG;
                        //set expected sender
                        runningThread->t_wait4sender = sender;
                        //set running thread to NULL
                        runningThread = NULL;
                    }
                }
                //handle accounting
                handle_accounting(thread);
            }
            //else if in user mode
            else{
                //save the running thread before passup (side-effect)
                struct tcb_t* accountThread = runningThread;
                //set the cause as reserved instruction
                runningThread->t_s.CP15_Cause = CAUSE_EXCCODE_SET(runningThread->t_s.CP15_Cause, EXC_RESERVEDINSTR);
                //if there is PGM trap manager set
                if(runningThread->t_pcb->pgmMgr != NULL){
                    //decrease the program counter by a word size. (due to the Link Return register behaviour)
                    runningThread->t_s.pc -= WORD_SIZE;
                    //pass up
                    trap_passup(runningThread->t_pcb->pgmMgr);
                    //handle accounting
                    handle_accounting(accountThread);
                }
                //otherwise
                else{
                    //terminate the process
                    proc_terminate(runningThread->t_pcb);
                }
            }
        }
        //foreign sys call
        else{
            //save the running thread before passup (side-effect)
            struct tcb_t* accountThread = runningThread;
            //if there is SYS/BK manager set
            if(runningThread->t_pcb->sysMgr != NULL){
                //pass up
                trap_passup(runningThread->t_pcb->sysMgr);
                //handle accounting
                handle_accounting(accountThread);
            }
            //else if there is PGM trap manager set
            else if(runningThread->t_pcb->pgmMgr != NULL){
                //decrease the program counter by a word size. (due to the Link Return register behaviour)
                runningThread->t_s.pc -= WORD_SIZE;
                //pass up
                trap_passup(runningThread->t_pcb->pgmMgr);
                //handle accounting
                handle_accounting(accountThread);
            }
            //otherwise
            else{
                //terminate the process
                proc_terminate(runningThread->t_pcb);
            }
        }

    }
    //else if the exception is a breakpoint
    else if (cause == EXC_BREAKPOINT){
        //TODO: handle
        
        //handle thread accounting
        handle_accounting(runningThread);
    }
    //else if something went wrong
    else{
        //print an error (here interrupts are masked)
        tprint("SYS/BK HANDLER: Something went wrong\n");
        //panic
        PANIC();
    }
    //call the scheduler
    scheduler();
}

void trap_passup(struct tcb_t* manager){
    //send a message to the manager with a pointer to the status as payload
    //and the offending thread as sender. Save the return value.
    int succeeded = do_send(runningThread, manager,  (uintptr_t) &(runningThread->t_s));
    //if send succeeded
    if(succeeded == 0){
        //enqueue in wait queue
        thread_enqueue(runningThread, &waitQueue);
        //increase soft blocked threads number
        softBlockedThread++;
        //set status as wait for trap
        runningThread->t_status = T_STATUS_W4TRAP;
        //remove the thread from running
        runningThread = NULL;
    }
    //else (message limit exceeded)
    else{
        //print error message and panic
        tprint("CAN'T PASS UP TRAP: send failed\n");
        PANIC();
    }
}

int do_send(struct tcb_t* src, struct tcb_t* dst, uintptr_t msg){
    //declare a variable for storing the function outcome
    int result;
    //if enqueuing the message and saving the result is successful
    if ((result=msgq_add(src, dst, msg))==0) {
        //if the destination thread is waiting for a message
        if ((dst->t_status == T_STATUS_W4MSG) && ((dst->t_wait4sender == NULL) || (dst->t_wait4sender == src))) {
            //remove dst from the wait queue
            list_del(&(dst->t_sched));
            //decrease soft blocked threads number
            softBlockedThread--;
            //move thread to readyQueue
            thread_enqueue(dst, &readyQueue);
            //set dst status to ready
            dst->t_status = T_STATUS_READY;
        }
    }
    //return the result of message enqueue
    return result;
}
