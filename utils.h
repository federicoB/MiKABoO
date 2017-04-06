#ifndef UTILS_H
#define UTILS_H

#include "listx.h"
#include "arch.h"

//a memory address in uARM
typedef unsigned int memaddress;

/**
 * Copies "size" bytes from source to destination.
 * Overlapping areas will lead to undefined behaviour.
 * @param src void*: pointer to the source area.
 * @param dst void*: pointer to the destination area.
 * @param size size_t: number of bytes to copy.
 */
void memcopy(void* src, void* dst, size_tt size);

/**
 * Returns the TODLO in microseconds.
 * @returns unsigned int: current TODLO in microseconds.
 */
#define TODLO_US() (getTODLO() / *((unsigned int*) BUS_REG_TIME_SCALE))

/**
 * Set the interval timer to the given value in microseconds.
 * @param microseconds unsigned int: timer value in microseconds.
 */
#define SET_IT_US(microseconds) setTIMER(microseconds * *((unsigned int*) BUS_REG_TIME_SCALE))


#endif /* UTILS_H */

