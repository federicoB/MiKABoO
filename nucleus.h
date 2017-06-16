#ifndef NUCLEUS_H
#define NUCLEUS_H

#include <stdint.h>
#include "mikabooq.h"

/* Syscall mnemonic values */
#define SYS_SEND    1
#define SYS_RECV    2

/* SSI requests */
#define GET_ERRNO 0
#define CREATE_PROCESS 1
#define CREATE_THREAD 2
#define TERMINATE_PROCESS 3
#define TERMINATE_THREAD 4
#define SETPGMMGR 5
#define SETTLBMGR 6
#define SETSYSMGR 7
#define GET_CPUTIME 8
#define WAIT_FOR_CLOCK 9
#define DO_IO 10
#define GET_PROCESSID 11
#define GET_MYTHREADID 12
#define GET_PARENTPROCID 13

typedef uintptr_t memaddr;
typedef uintptr_t cputime;
typedef uintptr_t devaddr;

extern void* SSI;

#define msgsend(dest, payload) (SYSCALL(SYS_SEND,(unsigned int) (dest),(unsigned int) (payload),0))

#define msgrecv(source, reply) (((struct tcb_t *) SYSCALL(SYS_RECV,(unsigned int) (source),(unsigned int) (reply),0)))

static inline uintptr_t geterrno(void) {
	uintptr_t retval;
	struct {
		uintptr_t reqtag;
	} req = {GET_ERRNO};
	msgsend(SSI, &req);
	msgrecv(SSI, &retval);
	return retval;
}

static inline struct tcb_t* create_process(state_t* s) {
	struct tcb_t* retval;
	struct {
		uintptr_t reqtag;
		state_t* s;
	} req = {CREATE_PROCESS, s};
	msgsend(SSI, &req);
	msgrecv(SSI, &retval);
	return retval;
}

static inline struct tcb_t* create_thread(state_t* s) {
	struct tcb_t* retval;
	struct {
		uintptr_t reqtag;
		state_t* s;
	} req = {CREATE_THREAD, s};
	msgsend(SSI, &req);
	msgrecv(SSI, &retval);
	return retval;
}

static inline void terminate_process(void) {
	struct {
		uintptr_t reqtag;
	} req = {TERMINATE_PROCESS};
	msgsend(SSI, &req);
	msgrecv(SSI, NULL);
}

static inline void terminate_thread(void) {
	struct {
		uintptr_t reqtag;
	} req = {TERMINATE_THREAD};
	msgsend(SSI, &req);
	msgrecv(SSI, NULL);
}

static inline struct tcb_t* setpgmmgr(struct tcb_t* s) {
	struct tcb_t* retval;
	struct {
		uintptr_t reqtag;
		struct tcb_t* s;
	} req = {SETPGMMGR, s};
	msgsend(SSI, &req);
	msgrecv(SSI, &retval);
	return retval;
}

static inline struct tcb_t* settlbmgr(struct tcb_t* s) {
	struct tcb_t* retval;
	struct {
		uintptr_t reqtag;
		struct tcb_t* s;
	} req = {SETTLBMGR, s};
	msgsend(SSI, &req);
	msgrecv(SSI, &retval);
	return retval;
}

static inline struct tcb_t* setsysmgr(struct tcb_t* s) {
	struct tcb_t* retval;
	struct {
		uintptr_t reqtag;
		struct tcb_t* s;
	} req = {SETSYSMGR, s};
	msgsend(SSI, &req);
	msgrecv(SSI, &retval);
	return retval;
}

static inline cputime getcputime(void) {
	uintptr_t retval;
	struct {
		uintptr_t reqtag;
	} req = {GET_CPUTIME};
	msgsend(SSI, &req);
	msgrecv(SSI, &retval);
	return retval;
}

static inline void waitforclock(void) {
	struct {
		uintptr_t reqtag;
	} req = {WAIT_FOR_CLOCK};
	msgsend(SSI, &req);
	msgrecv(SSI, NULL);
}

static inline unsigned int do_io(devaddr device, uintptr_t command, uintptr_t data1, uintptr_t data2) {
	uintptr_t retval;
	struct {
		uintptr_t reqtag;
		devaddr device;
		uintptr_t command;
		uintptr_t data1;
		uintptr_t data2;
	} req = {DO_IO, device, command, data1, data2};
	msgsend(SSI, &req);
	msgrecv(SSI, &retval);
	return retval;
}

#define do_disk_io(dev, cmd, d1) do_io((dev),(cmd),(d1),0)
#define do_tape_io(dev, cmd, d1) do_io((dev),(cmd),(d1),0)
#define do_net_io(dev, cmd, d1, d2) do_io((dev),(cmd),(d1),(d2))
#define do_printer_io(dev, cmd, d1) do_io((dev),(cmd),(d1),0)
#define do_terminal_io(dev, cmd) do_io((dev),(cmd),0,0)

static inline struct pcb_t* get_processid(struct tcb_t* s) {
	struct pcb_t* retval;
	struct {
		uintptr_t reqtag;
		struct tcb_t* s;
	} req = {GET_PROCESSID, s};
	msgsend(SSI, &req);
	msgrecv(SSI, &retval);
	return retval;
}

static inline struct pcb_t* get_parentprocid(struct pcb_t* s) {
	struct pcb_t* retval;
	struct {
		uintptr_t reqtag;
		struct pcb_t* s;
	} req = {GET_PARENTPROCID, s};
	msgsend(SSI, &req);
	msgrecv(SSI, &retval);
	return retval;
}

static inline struct tcb_t* get_mythreadid(void) {
	struct tcb_t* retval;
	struct {
		uintptr_t reqtag;
	} req = {GET_MYTHREADID};
	msgsend(SSI, &req);
	msgrecv(SSI, &retval);
	return retval;
}

#endif
