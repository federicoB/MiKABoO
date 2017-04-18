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

/**
 * Enqueues the message to the recipient and wakes him up if necessary.
 * @param src tcb_t*: sender of the message.
 * @param dst tcb_t*: recipient of the message.
 * @param msg uintptr_t: message payload.
 * @return int: 0 in case of success or -1 in case of failure (e.g.: no more msg_t are available).
 */
int do_send(struct tcb_t* src, struct tcb_t* dst, uintptr_t msg);
#endif /* EXCEPTION_H */

