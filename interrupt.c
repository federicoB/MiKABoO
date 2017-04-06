#include "interrupt.h"
#include "scheduler.h"
#include "arch.h"
#include "utils.h"
#include "uARMconst.h"

int get_highest_priority_interrupt(unsigned int* bitmap){
	//power of 2 bit mask
    unsigned int mask = 1;
    //position of the first least significant(LSB) bit that equals 1 
    int pos = 0;
    if(*bitmap != 0){
    	//while LSB that equals 1 not found
	    while((*bitmap & mask)==0){
	    	//shift the bit mask by one bit
	        mask<<=1;
	        //increment the LSB's position
	        pos++;
	    }
	}
	//special case: bitmap == 0
	else{
		//invalid value, which happens only if bitmap is 0
		pos = -1;
	}
	//return the correct position
    return pos;
}

void int_handler(){
    //TODO: implement
    
    //if there was a thread running
    if(runningThread != NULL){
        //copy the old state in the tcb
        memcopy((state_t*) INT_OLDAREA, &(runningThread->t_s), sizeof(state_t));
        //handle thread accounting
        handle_accounting();
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
        tprint("INTERRUPT HANDLER: Something went wrong\n");
        //panic
        PANIC();
    }
    //call the scheduler
    scheduler();
        
}
