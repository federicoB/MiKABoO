#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "mikabooq.h"

//ready threads queue
struct list_head readyQueue;

//waiting threads queue
struct list_head waitQueue;

//running thread
struct tcb_t* runningThread;

//total number of threads
int totalThread;

//number of soft-blocked threads
int softBlockedThread;

void sched_init();
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
 */
void handle_accounting();


#endif /* SCHEDULER_H */
