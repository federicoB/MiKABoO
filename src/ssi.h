#ifndef SSI_H
#define SSI_H

#include "src/mikabooq.h"
#include "scheduler.h"

/**
 * tcb_t*: pointer to the SSI.
 */
struct tcb_t* SSI;

#define SSI_addr SSI

/**
 * System Service Interface entry point.
 */
void SSI_entry_point();

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
 * Suspends the given thread until the next pseudoclock tick.
 * @param thread tcb_t*: thread requesting the service.
 */
void wait_for_clock(struct tcb_t* thread);

/**
 * Get device number and interrupt line number given an address of a device memory register.
 * @param address unsigned int: device register address (STATUS of COMMAND doesn't matter)
 * @param lineNumber unsigned int*: a pointer to an area where the device number will be saved
 * @param deviceNumber unsigned int*: a pointer to an area where the line number will be saved
 * @return 0 if address is correct address, -1 otherwise
 */
int getDeviceLineNumber(unsigned int address,unsigned int* lineNumber, unsigned int* deviceNumber);

/**
 * Set I/O parameters into device memory area
 * @param sender tcb_t*: the thread requesting I/O
 * @param payload void*: pointer to ssi received message. Must contain I/O parameters.
 * @return 0 if function execution is successful, -1 otherwise
 */
int do_io_ssi(struct tcb_t* sender, void* payload);

/**
 * Returns the CPU execution time of the given process.
 * @param applicant pcb_t*: the process.
 * @return unsigned int: the time.
 */
unsigned int get_CPU_time(struct pcb_t* applicant);
#endif /* SSI_H */

