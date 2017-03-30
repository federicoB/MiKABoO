#ifndef SSI_H
#define SSI_H

#include "mikabooq.h"
#include "scheduler.h"

/**
 * An SSI request message.
 */
struct ssimsg_t{
    
    /**
     * unsigned int: the requested service.
     */
    unsigned int service;
    
    /**
     * unsigned int: the argument if needed.
     */
    unsigned int argument;
};

/**
 * tcb_t*: pointer to the SSI.
 */
struct tcb_t* SSI_addr;

/**
 * System Service Interface entry point.
 */
void SSI_entry_point();

/**
 * Sends an SSI request with the given information.
 * @param service unsigned int: SSI service mnemonic code.
 * @param payload unsigned int: argument for the service (if required).
 * @param reply unsigned int*: pointer to the area where the answer should be stored (if necessary).
 */
void SSIRequest(unsigned int service, unsigned int payload, unsigned int* reply);

/**
 * Terminates the given process and its whole process tree.
 * Termination will also delete every process's thread.
 * @param proc pcb_t*: pointer to the process to delete.
 */
void proc_terminate(struct pcb_t* proc);

/**
 * Terminates the given thread.
 * Termination will also discard all pending messages.
 * @param thread tcb_t*: pointer to the thread to delete.
 */
void thread_terminate(struct tcb_t* thread);

/**
 * Creates a thread with the given state and the given process as parent.
 * @param process pcb_t*: pointer to the parent process.
 * @param state state_t*: pointer to the initial thread state.
 * @return struct tcb_t*: a pointer to the created thread if successful, CREATENOGOOD otherwise.
 */
struct tcb_t* thread_create(struct pcb_t* process, state_t* state);

/**
 * Creates a process child of the given parent and allocates its first thread with the given state.
 * @param parent pcb_t*: pointer to the parent process.
 * @param state state_t*: pointer to the state of the first thread.
 * @return struct pcb_t*: a pointer to the created process if successful, CREATENOGOOD otherwise.
 */
struct pcb_t* proc_create(struct pcb_t* parent, state_t* state);

/**
 * Sets the given SYS/BK manager to the process of the given thread.
 * If the already set, terminates the process.
 * @param applicant tcb_t*: the applicant thread.
 * @param manager tcb_t*: the manager thread.
 * @return int: 0 if succeeded, -1 otherwise.
 */
int set_SYSBK_manager(struct tcb_t* applicant, struct tcb_t* manager);

/**
 * Sets the given TLB manager to the process of the given thread.
 * If the already set, terminates the process.
 * @param applicant tcb_t*: the applicant thread.
 * @param manager tcb_t*: the manager thread.
 * @return int: 0 if succeeded, -1 otherwise.
 */
int set_TLB_manager(struct tcb_t* applicant, struct tcb_t* manager);

/**
 * Sets the given PGM manager to the process of the given thread.
 * If the already set, terminates the process.
 * @param applicant tcb_t*: the applicant thread.
 * @param manager tcb_t*: the manager thread.
 * @return int: 0 if succeeded, -1 otherwise.
 */
int set_PGM_manager(struct tcb_t* applicant, struct tcb_t* manager);

/**
 * Returns the CPU execution time of the given process.
 * @param applicant pcb_t*: the process.
 * @return unsigned int: the time.
 */
unsigned int get_CPU_time(struct pcb_t* applicant);
#endif /* SSI_H */

