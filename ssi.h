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

#endif /* SSI_H */

