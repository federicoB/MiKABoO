#ifndef UTILS_H
#define UTILS_H

#include "listx.h"

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

#endif /* UTILS_H */

