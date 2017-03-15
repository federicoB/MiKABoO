#include <arch.h>
#include "mikabooq.h"
#include "scheduler.h"
#include "base.h"

//declare an extern test function
extern void test();


void setHandler(memaddress newArea, memaddress handler){
    //cast the given area address as a pointer to state_t
    state_t* newAreaP = (state_t *) newArea;
    //init the area with the current state
    STST(newAreaP);
    //set the program counter as the address of the handler
    newAreaP->pc = handler;
    //set the stack pointer as the ram top
    newAreaP->sp = RAM_TOP;
    //set all interrupts as masked and set kernel mode
    newAreaP->cpsr = STATUS_ALL_INT_DISABLE((newAreaP->cpsr) | STATUS_SYS_MODE);
    //disable VM
    newAreaP->CP15_Control = CP15_DISABLE_VM(newAreaP->CP15_Control);
}

int main(int argc, char* * argv[]) {
    //get root process
    struct pcb_t* rootPCB = proc_init();
    //initialize threads
    thread_init();
    //initialize inter-process communication
    msgq_init();
    //initialize processes' ready queue
    INIT_LIST_HEAD(&readyQueue);
    //initialize processes' wait queue
    INIT_LIST_HEAD(&waitQueue);
    //create a new process with the root process as parent
    struct pcb_t* newProc = proc_alloc(rootPCB);
    //create a thread for the previously created process
    struct tcb_t* thread = thread_alloc(newProc);
    //enable interrupts and set kernel mode
    thread->t_s.cpsr = STATUS_ALL_INT_ENABLE(thread->t_s.cpsr) | STATUS_SYS_MODE;
    //set the correct stack pointer (Top of the memory - size of a frame, leaving enough space for the kernel stack)
    thread->t_s.sp = RAM_TOP - FRAME_SIZE;
    //set program counter as the address of the first instruction of the test program
    thread->t_s.pc = (memaddress) test;
    //enqueue the thread in the ready queue
    sched_enqueue(&readyQueue, thread);
    //call the scheduler
    scheduler();
    //this point should never be reached
    return 0;
}
