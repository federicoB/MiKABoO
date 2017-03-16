#ifndef EXCEPTION_H
#define EXCEPTION_H

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

#endif /* EXCEPTION_H */

