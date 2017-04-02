#ifndef EXCEPTION_H
#define EXCEPTION_H

#include "mikabooq.h"

/**
 * TLB exception handler routine.
 */
void tlb_handler();

/**
 * PGM trap handler routine.
 */
void pgm_trap_handler();

/**
 * System call/breakpoint handler routine.
 */
void sys_bk_handler();

/**
 * Passes up the exception to the given manager.
 * This function also moves the running thread to the wait queue and wakes the manager up.
 * @param manager struct tcb_t*: the trap manager.
 */
void trap_passup(struct tcb_t* manager);

#endif /* EXCEPTION_H */

