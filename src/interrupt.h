#ifndef INTERRUPT_H
#define INTERRUPT_H

#include "listx.h"

/**
 * Matrix for keep record of threads waiting for a device I/O.
 * Rows are device number [0..7] and columns are interrupt line number (device type).
 */
struct tcb_t* threadsWaitingDevices[8][5];

/**
 * Initialize data structures used for interrupt handling.
 */
void init_interrupt_handler();

/**
 * Returns the position of first least significant bit
 * that equals 1 in the given interrupt line bitmap.
 * @param bitmap unsigned int*: interrupt line bitmap.
 * @return int: the position of first LSB that equals 1, or -1 if the given bitmap is 0.
 */
int get_highest_priority_interrupt(unsigned int* bitmap);

/**
 * Interrupt handler routine.
 */
void int_handler();

/**
 * Handles interrupts from standard devices including terminals.
 * @param lineNumber int: the interrupt line number.
 */
void standard_device_handler(int lineNumber);

#endif /* INTERRUPT_H */

