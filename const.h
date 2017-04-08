#ifndef CONST_H
#define CONST_H

#include "mikabooq.h"

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
 * Mnemonic SSI service code: child process creation.
 */
#define CREATE_PROCESS 1

/**
 * Mnemonic SSI service code: brother thread creation.
 */
#define CREATE_THREAD 2

/**
 * Mnemonic SSI service code: process termination.
 */
#define TERMINATE_PROCESS 3

/**
 * Mnemonic SSI service code: thread termination.
 */
#define TERMINATE_THREAD 4

/**
 * Mnemonic SSI service code: set process-specific PGM trap manager.
 */
#define SETPGMMGR 5

/**
 * Mnemonic SSI service code: set process-specific TLB trap manager.
 */
#define SETTLBMGR 6

/**
 * Mnemonic SSI service code: set process-specific SYS/BK trap manager.
 */
#define SETSYSMGR 7

/**
 * Mnemonic SSI service code: get cpu execution time.
 */
#define GETCPUTIME 8

/**
 * Mnemonic SSI service code: wait for pseudoclock tick.
 */
#define WAIT_FOR_CLOCK 9

/**
 * Mnemonic SSI service code: send IO command and wait.
 */
#define WAIT_FOR_IO 10

/**
 * Mnemonic SSI service code: get the process's number.
 */
#define GET_PROCESS_N 11

/**
 * Mnemonic SSI service code: get the thread's number.
 */
#define GET_THREAD_N 12

/**
 * MiKaBoO time slice in microseconds.
 */
#define TIME_SLICE 5000

/**
 * MiKaBoO pseudoclock tick time in microseconds.
 */
#define PSEUDO_TICK 100000

/**
 * Generic device command register offset.
 */
#define GENERIC_COMMAND 0x00000004

/**
 * Terminal read status register offset.
 */
#define TERM_READ_STATUS 0x00000000

/**
 * Terminal read command register offset.
 */
#define TERM_READ_COMMAND GENERIC_COMMAND

/**
 * Terminal write status register offset.
 */
#define TERM_WRITE_STATUS 0x00000008

/**
 * Terminal write command register offset.
 */
#define TERM_WRITE_COMMAND 0x0000000C

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
    ((struct tcb_t*) SYSCALL((unsigned int) RECV,(unsigned int) source,(unsigned int) payloadP, NULL))

#endif
