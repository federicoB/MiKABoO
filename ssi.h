#ifndef SSI_H
#define SSI_H

#include "mikabooq.h"
#include "scheduler.h"

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

#endif /* SSI_H */

