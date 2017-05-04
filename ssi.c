#include <libuarm.h>
#include "ssi.h"
#include "const.h"
#include "utils.h"
#include "interrupt.h"
#include "uARMconst.h"

void SSI_entry_point(){
    //forever (until killed)
    while(1){
        //pointer to the ssi msg
        //struct ssimsg_t* msg = NULL;
        //receive a message and save the sender
        //struct tcb_t* thread = MsgRecv(NULL, &msg);
        void* payload;

        struct tcb_t* thread = MsgRecv(NULL, &payload);
        //enter critical section
        setSTATUS(STATUS_ALL_INT_DISABLE(getSTATUS()));
        unsigned int* service = payload;
        //case: get error number.
        if(*service == GET_ERRNO){
            //get a copy of the error number
            int errno = thread->errno;
            //set errno as 0 (success)
            thread->errno = 0;
            //Send a message with the error number as payload
            MsgSend(thread, errno);

        }
        //case: create a process.
        else if(*service == CREATE_PROCESS){
            struct {
                unsigned int service;
                state_t* thread_state;
            }* message = payload;
            //create the process and its first thread with the given state
            struct pcb_t* proc = proc_create(thread->t_pcb, message->thread_state);
            //first created thread
            struct tcb_t* createdThread = NULL;
            //if succeded
            if(proc != NULL){
                //set errno as 0 (success)
                thread->errno = 0;
                //save created thread
                createdThread = proc_firstthread(proc);
            }
            //otherwise (error)
            else{
                //TODO: set errno as error
            }
            //send the reply with the created thread as payload (or NULL if failed)
            MsgSend(thread, createdThread);
        }
        //case: create a thread.
        else if(*service == CREATE_THREAD){
            struct {
                unsigned int service;
                state_t* thread_state;
            }* message = payload;
            //create the thread with the given state
            struct tcb_t* createdThread = thread_create(thread->t_pcb, message->thread_state);
            //if succeded
            if(createdThread != NULL){
                //set errno as 0 (success)
                thread->errno = 0;
            }
            //otherwise (error)
            else{
                //TODO: set errno as error
            }
            //send the reply with the created thread as payload (or NULL if failed)
            MsgSend(thread, createdThread);
        }
        //case: terminate the process
        else if(*service == TERMINATE_PROCESS){
            //terminate the process
            proc_terminate(thread->t_pcb);
        }
        //case: terminate the thread
        else if(*service == TERMINATE_THREAD){        
            //if is the last thread of the process
            if(thread->t_pcb->p_threads.next == thread->t_pcb->p_threads.prev){
                //terminate the whole process
                proc_terminate(thread->t_pcb);
            }
            //otherwise (more than 1 thread)
            else{
                //terminate the thread
                thread_terminate(thread);
            }
        }
        //case: set PGM manager.
        else if(*service == SETPGMMGR){
            //TODO: compress code, avoid copy-paste.
            struct {
                unsigned int service;
                struct tcb_t* manager;
            }* message = payload;
            //set the manager
            int result = set_PGM_manager(thread, message->manager);
            //if succeeded
            if(result == 0){
                //set errno as 0 (success)
                thread->errno = 0;
                //send the reply with the given manager as payload
                MsgSend(thread, message->manager);
            }
        }
        //case: set TLB trap manager.
        else if(*service == SETTLBMGR){
            //TODO: compress code, avoid copy-paste.
            struct {
                unsigned int service;
                struct tcb_t* manager;
            }* message = payload;
            //set the manager
            int result = set_TLB_manager(thread, message->manager);
            //if succeeded
            if(result == 0){
                //set errno as 0 (success)
                thread->errno = 0;
                //send the reply with the given manager as payload
                MsgSend(thread, message->manager);
            }
        }
        //case: set SYS/BK trap manager.
        else if(*service == SETSYSMGR){
            //TODO: compress code, avoid copy-paste.
            struct {
                unsigned int service;
                struct tcb_t* manager;
            }* message = payload;
            //set the manager
            int result = set_SYSBK_manager(thread, message->manager);
            //if succeeded
            if(result == 0){
                //set errno as 0 (success)
                thread->errno = 0;
                //send the reply with the given manager as payload
                MsgSend(thread, message->manager);
            }
        }
        //case: get CPU execution time.
        else if(*service == GETCPUTIME){
            //set errno as 0 (success)
            thread->errno = 0;
            //Send a message with the CPU time as payload
            MsgSend(thread, thread->t_pcb->CPU_time);
        }
        //case:wait for next pseudoclick tick
        else if(*service == WAIT_FOR_CLOCK){
            //insert the thread inside the pseudoclock list
            wait_for_clock(thread);
            //NOTE: a message reply will be triggered asyncronously by the kernel.
        }
        //case: do IO and wait.
        else if(*service == DO_IO){
            //set I/O parameters into device memory area
            do_io_ssi(thread, payload);
        }
        //case: get process ID.
        else if(*service == GET_PROCESSID){
            struct {
                uintptr_t reqtag;
                //ssi service will answer the process of this thread
                struct tcb_t* thread;
            }* message = payload;
            //set errno as 0 (success)
            thread->errno = 0;
            //Send a message with the process ID of the given thread as payload
            MsgSend(thread, message->thread->t_pcb);
        }
        //case: get thread ID.
        else if(*service == GET_THREADID){
            //set errno as 0 (success)
            thread->errno = 0;
            //Send a message with the process ID as payload
            MsgSend(thread, thread);
        }
        //case: get parent process ID.
        else if(*service == GET_PARENTPROCESSID){
            struct {
                uintptr_t reqtag;
                //ssi service will aswer the parent process of this process
                struct pcb_t* process;
            }* message = payload;
            //set errno as 0 (success)
            thread->errno = 0;
            //Send a message with the parent process ID as payload
            MsgSend(thread, message->process->p_parent);
        }
        //default: error.
        else{
            //print an error messsage and panic
            tprint("Invalid SSI service\n");
            PANIC();
        }
        //exit critical section
        setSTATUS(STATUS_ALL_INT_ENABLE(getSTATUS()));
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
    //the process has no children at this point
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
        if (thread_create(process, state) == NULL) {
            //delete the process
            proc_delete(process);
            //set the process address to an error code
            process = NULL;
        }
    }
    //else if process allocation failed
    else {
        //set the process address to an error code
        process = NULL;
    }
    //return the new process
    return process;
}

void thread_terminate(struct tcb_t* thread){
    //if in wait queue (not in ready and not unused)
    if(thread->t_status != T_STATUS_READY){
        //decrease softblock
        softBlockedThread--;
        //update device wait structures
        //declare matrix iterators
        int i,j;
        //for each row
        for (i=0;i<8;i++) {
            //for each column
            for (j=0;j<5;j++) {
                //if this thread is waiting for a device
                if (threadsWaitingDevices[i][j]==thread) {
                    //remove thread from matrix
                    threadsWaitingDevices[i][j]=NULL;
                }
            }
        }
        //remove from pseudoclock list
        //if the hook is not pinting to itself
        if(!list_empty(&(thread->t_pseudo))){
            //remove from the pseudoclock list
            list_del(&(thread->t_pseudo));
        }
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
    //handle not yet read messages with this thread as a sender
    //declare iterators
    struct msg_t* msg_it;
    struct msg_t* msg_it_next;
    //foreach sent but not yet read message (use safe macro)
    list_for_each_entry_safe(msg_it, msg_it_next, &(thread->t_sentmsg), m_tnext){
        //free the message
        msg_free(msg_it);
    }     
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
        //set the thread address as NULL
        thread = NULL;
    }
    //return the result
    return thread;
}

void wait_for_clock(struct tcb_t* thread){
    //add the thread to the pseudoclock list
    list_add_tail(&(thread->t_pseudo),&pseudoClockList);
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

int getDeviceLineNumber(unsigned int address, unsigned int* lineNumber, unsigned int* deviceNumber) {
    //initialize lineNumber number to 3
    *lineNumber = 3;
    //initialize deviceNumber number to 0
    *deviceNumber = 0;
    //if the device is a disk, tape, network, printer or a terminal
    if (address >= DEV_REG_ADDR(IL_DISK, 0)
        && (address) <= DEV_REG_ADDR(IL_TERMINAL, 7) + TERM_WRITE_COMMAND) {
        //devices registers start from address 64. Subtract it.
        int signedAddress = address - DEV_REG_START;
        //first subtract all below lineNumber address and then all below deviceNumber address
        //address space for a line is the address space of a device multiplied by number of device per int line
        const unsigned int lineSize = DEV_REG_SIZE * DEV_PER_INT;
        //multiple subtraction algorithm
        while (signedAddress >= lineSize) {
            //increase line number value
            (*lineNumber)++;
            //subtract lineSize
            signedAddress -= lineSize;
        }
        //multiple subtraction algorithm
        while (signedAddress >= DEV_REG_SIZE) {
            //increase device number value
            (*deviceNumber)++;
            //subtract device register size
            signedAddress -= DEV_REG_SIZE;
        }
        //return correct exit code
        return 0;
    } else {
        //IPI, CPUTIMER and TIMER are not implented in this version
        //return an error code
        return -1;
    }
}

int do_io_ssi(struct tcb_t *sender, void *payload) {
    //declare a common structure among the messages
    struct {
        //the requested ssi service
        unsigned int service;
        //the address of the device command area
        unsigned int* deviceAddress;
        //the requested command
        unsigned int command;
    }* message = payload;
    //declare a variable for device number and line number
    unsigned int deviceNumber, lineNumber;
    //get device and line number and save the function result
    int result = getDeviceLineNumber((unsigned int)message->deviceAddress, &lineNumber, &deviceNumber);
    //if getDeviceLineNumber is successful
    if (result==0) {
        //if the requested device is a disk, tape, network or a printer
        if ((message->deviceAddress) >= (memaddress*) DEV_REG_ADDR(IL_DISK, 0)
            && (message->deviceAddress) <= (memaddress*) DEV_REG_ADDR(IL_PRINTER, 7) + TERM_WRITE_COMMAND) {
            struct {
                //the requested ssi service
                unsigned int service;
                //the address of the device command area
                unsigned int* deviceAddress;
                //the requested command
                unsigned int command;
                //data 1 device parameter
                unsigned int data1;
            } * message = payload;
            //copy data1 into a corresponding area located 4 byte ahead of command area of device memory register
            *(message->deviceAddress + 1) = message->data1;
            //if is an ethernet device
            if (((message->deviceAddress) >= (memaddress*) DEV_REG_ADDR(IL_ETHERNET, 0) + GENERIC_COMMAND)
                && (message->deviceAddress) <= (memaddress*) DEV_REG_ADDR(IL_ETHERNET, 7) + GENERIC_COMMAND) {
                struct {
                    //the requested ssi service
                    unsigned int service;
                    //the address of the device command area
                    unsigned int* deviceAddress;
                    //the requested command
                    unsigned int command;
                    //ethernet data1 parameter
                    unsigned int data1;
                    //ethernet data2 MAC address parameter
                    unsigned int data2;
                } * message = payload;
                //copy data2 into a corresponding area located 8 byte ahead of command area of device memory register
                *(message->deviceAddress + 2) = message->data2;
            }
        }
        //decrease line number by 3 to match compact matrix index
        lineNumber -= 3;
        //set pending request matrix with requesting thread
        threadsWaitingDevices[deviceNumber][lineNumber] = sender;
        //set command into the correct area of device memory register
        //the device now will execute the request
        *(message->deviceAddress) = message->command;
    }
    //set the error number of sender thread
    sender->errno = result;
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