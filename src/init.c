#include <libuarm.h>
#include <arch.h>
#include "mikabooq.h"
#include "scheduler.h"
#include "utils.h"
#include "exception.h"
#include "interrupt.h"
#include "ssi.h"

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

int main(int argc, char** argv[]) {
    //get root process
    struct pcb_t* rootPCB = proc_init();
    //initialize threads
    thread_init();
    //initialize inter-process communication
    msgq_init();
    //initialize scheduler data structures
    sched_init();
    //initialize interrupt handling
    init_interrupt_handler();
    //set syscall/breakpoint handler
    setHandler(SYSBK_NEWAREA, (memaddress) sys_bk_handler);
    //set PGM trap handler
    setHandler(PGMTRAP_NEWAREA, (memaddress) pgm_trap_handler);
    //set TLB exception handler
    setHandler(TLB_NEWAREA, (memaddress) tlb_handler);
    //set interrupt handler
    setHandler(INT_NEWAREA, (memaddress) int_handler);
    //create a new process for the ssi with the root process as parent
    struct pcb_t* newProc = proc_alloc(rootPCB);
    //create the ssi thread for the previously created process and save globally its address
    SSI_addr = thread_alloc(newProc);
    //enable interrupts and set kernel mode
    SSI_addr->t_s.cpsr = STATUS_ALL_INT_ENABLE(SSI_addr->t_s.cpsr) | STATUS_SYS_MODE;
    //set the correct stack pointer (Top of the memory - size of a frame, leaving enough space for the kernel stack)
    SSI_addr->t_s.sp = RAM_TOP - FRAME_SIZE;
    //set program counter as the address of the first instruction of the test program
    SSI_addr->t_s.pc = (memaddress) SSI_entry_point;
    //enqueue the thread in the ready queue
    thread_enqueue(SSI_addr, &readyQueue);
    //create another new process with the root process as parent
    newProc = proc_alloc(rootPCB);
    //create a thread for the previously created process
    struct tcb_t* thread = thread_alloc(newProc);
    //enable interrupts and set kernel mode
    thread->t_s.cpsr = STATUS_ALL_INT_ENABLE(thread->t_s.cpsr) | STATUS_SYS_MODE;
    //set the correct stack pointer (Top of the memory - 2 * size of a frame, leaving enough space for the kernel and ssi stack)
    thread->t_s.sp = RAM_TOP - (2*FRAME_SIZE);
    //set program counter as the address of the first instruction of the test program
    thread->t_s.pc = (memaddress) test;
    //increase the total number of thread
    totalThread+=2;
    //enqueue the thread in the ready queue
    thread_enqueue(thread, &readyQueue);

    //call the scheduler
    scheduler();
    //this point should never be reached
    return 0;
}
