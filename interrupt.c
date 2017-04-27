#include <libuarm.h>
#include "interrupt.h"
#include "scheduler.h"
#include "arch.h"
#include "utils.h"
#include "uARMconst.h"
#include "const.h"
#include "exception.h"
#include "ssi.h"

int get_highest_priority_interrupt(unsigned int* bitmap){
    //power of 2 bit mask
    unsigned int mask = 1;
    //position of the first least significant(LSB) bit that equals 1 
    int pos = 0;
    //if the given bitmap is not zero
    if(*bitmap != 0){
        //while the masked bitmap equals zero
        while((*bitmap & mask) == 0){
            //shift the bit mask by one bit
            mask <<= 1;
            //increment the LSB position
            pos++;
        }
    }
    //special case: bitmap == 0
    else{
        //invalid value, which happens only if bitmap is 0
        pos = -1;
    }
    //return the position
    return pos;
}

void int_handler(){
    //if there was a thread running
    if(runningThread != NULL){
        //copy the old state in the tcb
        memcopy((state_t*) INT_OLDAREA, &(runningThread->t_s), sizeof(state_t));
        //decrease the program counter by a word size. (due to the Link Return register behaviour)
        runningThread->t_s.pc -= WORD_SIZE;
        //handle thread accounting
        handle_accounting(runningThread);
    }
    //NOTE: more than one line and more than one device may have raised an interrupt.
    //Cause check is done in ascending priority order to handle the interrupt with highest priority first.
    //Terminals have 2 sub-devices: a reader and a writer. The latter has higher priority.
    
    //get the cause
    int IPCause = getCAUSE();
    //if there is an interrupt raised in the inter processor interrupt line
    if(CAUSE_IP_GET(IPCause, IL_IPI)){
        //NOT IMPLEMENTED IN THIS VERSION. Currently in single core mode only.
    }
    //else if there is an interrupt in the cpu timer line
    else if(CAUSE_IP_GET(IPCause, IL_CPUTIMER)){
        //NOT IMPLEMENTED IN THIS VERSION Currently in single core mode only.
    }
    //else if there is an interrupt raised in the interval timer line
    else if(CAUSE_IP_GET(IPCause, IL_TIMER)){
        //NOTE: at the moment accounting and pseudoclock handling are done regardless
        //of the actual interrupt type.
        //This block is kept here for future use.
    }
    //else if there is an interrupt raised in the disk line
    else if(CAUSE_IP_GET(IPCause, IL_DISK)){
        //use standard device handler
        standard_device_handler(IL_DISK);
    }
    //else if there is an interrupt raised in the tape line
    else if(CAUSE_IP_GET(IPCause, IL_TAPE)){
        //use standard device handler
        standard_device_handler(IL_TAPE);
    }
    //else if there is an interrupt raised in the network line
    else if(CAUSE_IP_GET(IPCause, IL_ETHERNET)){
        //use standard device handler
        standard_device_handler(IL_ETHERNET);
    }
    //else if there is an interrupt raised in the printer line
    else if(CAUSE_IP_GET(IPCause, IL_PRINTER)){
        //use standard device handler
        standard_device_handler(IL_PRINTER);
    }
    //else if there is an interrupt raised in the terminal line
    else if(CAUSE_IP_GET(IPCause, IL_TERMINAL)){
        //use standard device handler
        standard_device_handler(IL_TERMINAL);
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

void standard_device_handler(int lineNumber){
    //get the interrupt line bitmap
    unsigned int* bitmap = (unsigned int*) CDEV_BITMAP_ADDR(lineNumber);
    //get the highest priority interrupt in the bitmap
    int device = get_highest_priority_interrupt(bitmap);
    //the device command register
    unsigned int* command;
    //the device status register
    unsigned int* status;
    //if the device is a terminal
    if(lineNumber == IL_TERMINAL){
        //Terminals have 2 sub-devices: a reader and a writer. The latter has higher priority.
        //compute the read status register address
        unsigned int* statusRead = (unsigned int*) (DEV_REG_ADDR(IL_TERMINAL, device) + TERM_READ_STATUS);
        //compute the write status register address
        unsigned int* statusWrite = (unsigned int*) (DEV_REG_ADDR(IL_TERMINAL, device) + TERM_WRITE_STATUS);   
	//if there is a write interrupt pending (higher priority)
	if((*(statusWrite) & 0x0F) == DEV_TTRS_S_CHARTRSM){
            //set command register as the one of write device
            command = (unsigned int*) (DEV_REG_ADDR(IL_TERMINAL, device) + TERM_WRITE_COMMAND);
            //set command register as the one of write device
            status = statusWrite;
        }
	//else if there is a read interrupt pending (lower priority)
	else if((*(statusRead) & 0x0F) == DEV_TRCV_S_CHARRECV){
            //set command register as the one of write device
            command = (unsigned int*) (DEV_REG_ADDR(IL_TERMINAL, device) + TERM_READ_COMMAND);
            //set command register as the one of write device
            status = statusRead;
        }
    }
    //otherwise if is a generic device
    else{
        //get the command register
        command = (unsigned int*) (DEV_REG_ADDR(lineNumber, device) + GENERIC_COMMAND);
        //get the status register
        status = (unsigned int*) DEV_REG_ADDR(lineNumber, device);
    }
    //map the line number to matrix index
    //The first three lines (IPI,CPUTIMER,TIMER) are unused in pending interrupt matrix
    lineNumber -= 3;
    //get the i/o requesting thread from the data structure used to keep track of pending request
    struct tcb_t* requestingThread = threadsWaitingDevices[device][lineNumber];
    //if there is a thread waiting for the device
    if (requestingThread!=NULL) {
        //wake up the requesting thread using a message
        //The thread is waiting from the ssi so it will pretend to be the ssi
        do_send(SSI_addr, requestingThread, (uintptr_t) *status);
        //reset the element of the matrix
        threadsWaitingDevices[device][lineNumber] = NULL;
    }
    //send ack to device (set ack in commend register)
    *command = DEV_C_ACK;
}
