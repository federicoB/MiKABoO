#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "mikabooq.h"

/**
 * list_head: ready threads queue.
 */
struct list_head readyQueue;

/**
 * list_head: waiting threads queue.
 */
struct list_head waitQueue;

/**
 * list_head: list of threads waiting for pseudoclock (entry point).
 */
struct list_head pseudoClockList;

/**
 * tcb_t*: current running thread.
 */
struct tcb_t* runningThread;

/**
 * int: total number of threads
 */
int totalThread;

/**
 * int: number of soft-blocked threads
 */
int softBlockedThread;

/**
 * Initialize scheduler data structures.
 */
void sched_init();

/**
 * Schedule!
 */
void scheduler();

/**
 * unsigned int: TODLO at the last thread schedule.
 */
unsigned int lastLoadTime;

/**
 * unsigned int: remaining time slice.
 */
unsigned int timeSliceLeft;

/**
 * unsigned int: last pseudoclock tick time.
 */
unsigned int lastTickTime;

/**
 * unsigned int: total number of ticks since the system boot.
 */
unsigned int totalTicks;

/**
 * Handles pseudoclock ticks.
 * If pseudoclock tick happened, wakes clock-waiting threads up.
 * Returns time until next pseudoclock tick.
 * @param TODLO unsigned int: time of day.
 * @return unsigned int: time until next pseudoclock tick.
 */
unsigned int handle_pseudoclock(unsigned int TODLO);

/**
 * Handles threads accounting.
 * Charges CPU time to time to the process.
 * Reduces the time slice of the thread.
 * @param thread tcb_t*: thread for accounting.
 */
void handle_accounting(struct tcb_t* thread);


#endif /* SCHEDULER_H */
