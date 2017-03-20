#include "exception.h"
#include "scheduler.h"

void tlb_handler(){
    //TODO: implement
}

void pgm_trap_handler(){
    //TODO: implement
}

void sys_bk_handler(){
    //TODO: implement
    
    //TODO: copy the old state in the tcb
    
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
        tprint("SYS/BK HANDLER: Something went wonng\n");
        //panic
        PANIC();
    }
}