#include "interrupt.h"
#include "scheduler.h"
#include "arch.h"

void int_handler(){
    //TODO: implement
    
    //if there was a thread running
    if(runningThread != NULL){
        //TODO: copy the old state in the tcb
    }
    //NOTE: more than one line and more than one device may have raised an interrupt.
    //Cause check is done in ascending priority order to handle the interrupt with highest priority first.
    //Terminals have 2 sub-devices: a reader and a writer. The latter has higher priority.
    
    //get the cause
    int IPCause = getCAUSE();
    //if there is an interrupt raised in the inter processor interrupt line
    if(CAUSE_IP_GET(IPCause, IL_IPI)){
        //TODO: handle
    }
    //else if there is an interrupt in the cpu timer line
    else if(CAUSE_IP_GET(IPCause, IL_CPUTIMER)){
        //TODO: handle
    }
    //else if there is an interrupt raised in the interval timer line
    else if(CAUSE_IP_GET(IPCause, IL_TIMER)){
        //TODO: handle
    }
    //else if there is an interrupt raised in the disk line
    else if(CAUSE_IP_GET(IPCause, IL_DISK)){
        //TODO: handle
    }
    //else if there is an interrupt raised in the tape line
    else if(CAUSE_IP_GET(IPCause, IL_TAPE)){
        //TODO: handle
    }
    //else if there is an interrupt raised in the network line
    else if(CAUSE_IP_GET(IPCause, IL_ETHERNET)){
        //TODO: handle
    }
    //else if there is an interrupt raised in the printer line
    else if(CAUSE_IP_GET(IPCause, IL_PRINTER)){
        //TODO: handle
    }
    //else if there is an interrupt raised in the terminal line
    else if(CAUSE_IP_GET(IPCause, IL_TERMINAL)){
        //TODO: handle
    }
    //else if something went wrong
    else{
        //print an error (here interrupts are masked)
        tprint("INTERRUPT HANDLER: Something went wonng\n");
        //panic
        PANIC();
    }
    //call the scheduler
    scheduler();
        
}