#ifndef CONST_H
#define CONST_H

/**
 * int: maximum number of processes in the system.
 */
#define MAXPROC 20

/**
 * int: maximum number of threads in the system.
 */
#define MAXTHREAD 30

/**
 * int: maximum number of messages in the system.
 */
#define MAXMSG 40

/**
 * Mnemonic send syscall code.
 */
#define SEND 1

/**
 * Mnemonic receive syscall code.
 */
#define RECV 2

/**
 * Error code for creation.
 * Used by the SSI.
 */
#define CREATENOGOOD -1

/**
 * Sends a message to the destination with the given payload.
 * Implemented with a system call.
 * @param destination tcb_t*: pointer to the destination thread.
 * @param payload uintptr_t: the message payload.
 */
#define MsgSend(destination, payload) \
    (SYSCALL((unsigned int) SEND,(unsigned int) destination,(unsigned int) payload, NULL))

/**
 * Receives a message from the given source.
 * Implemented with a system call.
 * @param source tcb_t*: requested source.
 * @param payloadP uintptr_t*: pointer to the area where the payload will be saved.
 * @return tcb_t*: sender thread.
 */
#define MsgRecv(source, payloadP) \
    ((tcb_t*) SYSCALL((unsigned int) RECV,(unsigned int) source,(unsigned int) payloadP, NULL))

/**
 * Copies "size" bytes from source to destination.
 * Overlapping areas will lead to undefined behaviour.
 * @param source void*: pointer to the source area.
 * @param dest void*: pointer to the destination area.
 * @param size size_t: number of bytes to copy.
 */
#define memcopy(source, dest, size) \
    char* src = (char*) source; \
    char* dst = (char*) dest; \
    unsigned int s = size; \
    while (s--){ \
        *src++ = *dst++; \
    }
#endif
