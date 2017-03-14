#include <arch.h>
#include "mikabooq.h"

int main(int argc, char* * argv[]) {
    //get first process
    struct pcb_t* rootPCB = proc_init();
    //initialize threads
    thread_init();
    //initialize inter-process communication
    msgq_init();
    //initialize processes' ready queue
    //initialize processes' wait queue
    //create a new process with the first process as parent
    struct pcb_t* newProc = proc_alloc(rootPCB);
    //create a thread for the previously created process
    struct tcb_t* thread = thread_alloc(newProc);
    //enable interrupts and set kernel mode
    thread->t_s.cpsr = STATUS_ALL_INT_ENABLE(thread->t_s.cpsr) | STATUS_SYS_MODE;
    //set the correct stack pointer. Top of the memory - size of a frame, for avoiding overwriting kernel stack
    thread->t_s.sp = RAM_TOP - FRAME_SIZE;
    //set program counter as the address of the first instruction of the test program
    //enqueue the thread in the ready queue
    //call the scheduler
    return 0;
}
