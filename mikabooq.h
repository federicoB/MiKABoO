#ifndef MIKABOOQ_H
#define MIKABOOQ_H

#include <listx.h>
#include <uARMtypes.h>
#include <stdint.h>

/**
 * A process control block.
 */
struct pcb_t {
    
    /**
     * pcb_t: a pointer to the parent process.
     * NULL if the root of the process tree
     */
    struct pcb_t* p_parent;
    
    /**
     * list_head: entry point of the threads list.
     */
    struct list_head p_threads;

    /**
     * list_head: entry point of the children processes list.
     * This implements the processes hierarchy.
     */
    struct list_head p_children;
    
    /**
     * list_head: link (or "hook") to siblings.
     * Used to add this pcb_t to a list.
     */
    struct list_head p_siblings;
    
    /**
     * tcb_t*: pointer to the process-specific PGM trap manager.
     */
    struct tcb_t* pgmMgr;
    
    /**
     * tcb_t*: pointer to the process-specific TLB trap manager.
     */
    struct tcb_t* tlbMgr;
    
    /**
     * tcb_t*: pointer to the process-specific SYS/BK trap manager.
     */
    struct tcb_t* sysMgr;
    
    /**
     * unsigned int: total cpu execution time for the process.
     * This includes all of its threads.
     */
    unsigned int CPU_time;
};

/**
 * int: status "unused thread descriptor".
 */
#define T_STATUS_NONE  0

/**
 * int: status "thread ready".
 */
#define T_STATUS_READY 1

/**
 * int: status "thread waiting for a message".
 */
#define T_STATUS_W4MSG 4

/**
 * A thread control block.
 */
struct tcb_t {
    
    /**
     * pcb_t: a pointer to the process.
     */
    struct pcb_t* t_pcb;
    
    /**
     * state_t: the cpu state clone.
     */
    state_t t_s ;

    int t_status;

    /**
     * tcb_t*: expected sender. NULL means anybody.
     * This is valid only if t_status == T_STATUS_W4MSG.
     */
    struct tcb_t* t_wait4sender;
    
    /**
     * list_head: link (or "hook") to the other elements of the list of threads in the same process.
     * Used to add this tcb_t to a process's threads list.
     */
    struct list_head t_next;
    
    /**
     * list_head: link (or "hook") to other elements on the same scheduling list.
     * Used to add this tcb_t to a scheduling list.
     */
    struct list_head t_sched;
    
    /**
     * list_head: entry point of the pending messages list.
     */
    struct list_head t_msgq;
};

/**
 * An IPC message.
 */
struct msg_t {
    
    /**
     * tcb_t*: the message sender.
     */
    struct tcb_t* m_sender;
    
    /**
     * uintptr_t: the message payload.
     */
    uintptr_t m_value;

    /**
     * list_head: link (or "hook") to the next pending message in queue.
     * Used to add this msg_t to a list (used as queue).
     */
    struct list_head m_next;
};

//processes management functions

/**
 * Initialize processes data structures and allocate the root process.
 * @return pcb_t*: pointer to the allocated root process.
 */
struct pcb_t* proc_init(void);

/**
 * Allocates a new empty pcb (as a child of p_parent).
 * @param p_parent pcb_t*: the parent process (should not be NULL).
 * @return pcb_t*: a pointer to the allocated process or NULL in case of errors (p_parent == NULL or no more pcb-s are available).
 */
struct pcb_t* proc_alloc(struct pcb_t* p_parent);

/**
 * Deletes a process (properly updating the process tree links).
 * Fails if oldproc has threads or children.
 * @param oldproc pcb_t: the process to remove.
 * @return int: 0 in case of success, -1 otherwise.
 */
int proc_delete(struct pcb_t* oldproc);

/**
 * Returns a pointer to the first child of the given process.
 * @param proc pcb_t: the process.
 * @return pcb_t*: pointer to the first child or NULL if the given process has no children.
 */
struct pcb_t* proc_firstchild(struct pcb_t* proc);

/**
 * Returns a pointer to the first thread of the given process.
 * @param proc pcb_t: the process.
 * @return tcb_t*: pointer to the first thread or NULL if the given process has no threads.
 */
struct tcb_t* proc_firstthread(struct pcb_t* proc);

//threads management functions

/**
 * Initialize threads data structures.
 */
void thread_init(void);

/**
 * Allocates a new empty tcb (as a thread of process).
 * NOTE: t_status must be manually set to the right value according to the actual
 * status. (E.g.: set to "ready" if the thread will be enqueued to the ready queue)
 * @param process pcb_t*: the process (should not be NULL).
 * @return tcb_t*: a pointer to the allocated tcb or NULL in case of errors (process == NULL or no more tcb-s are available).
 */
struct tcb_t* thread_alloc(struct pcb_t* process);

/**
 * Deallocates a thread (unregistering it from the list of threads of its process).
 * Fails if the message queue is not empty.
 * @param oldthread tcb_t: the thread to deallocate.
 * @return int: 0 in case of success, -1 otherwise.
 */
int thread_free(struct tcb_t *oldthread);

/**
 * Enqueues the given thread in the given queue.
 * @param new tcb_t*: the thread.
 * @param queue list_head*: the queue entry point.
 */
void thread_enqueue(struct tcb_t* new, struct list_head* queue);

/**
 * Returns the head element of a scheduling queue.
 * @param queue list_head*: queue entry point.
 * @return tcb_t*: a pointer to the head element or NULL if the queue is empty.
 */
struct tcb_t* thread_qhead(struct list_head* queue);

/**
 * Dequeue the first element of a scheduling queue.
 * @param queue
 * @return tcb_t*: a pointer to the head element or NULL if the queue is empty.
 */
struct tcb_t* thread_dequeue(struct list_head* queue);

/**
 * Useful foreach macro for threads in queue.
 * @param pos tcb_t*: iterator.
 * @param queue list_head*: queue entry point.
 */
#define for_each_thread_in_q(pos, queue) \
	list_for_each_entry(pos, queue, t_sched)

//threads management functions

/**
 * Initialize messages data structures.
 */
void msgq_init();

/**
 * Adds a message to the destination message queue with the given data.
 * @param sender tcb_t*: message sender.
 * @param destination tcb_t*: message destination.
 * @param value uintptr_t: the payload.
 * @return int: 0 in case of success or -1 in case of failure (e.g.: no more msg_t are available).
 */
int msgq_add(struct tcb_t* sender, struct tcb_t* destination, uintptr_t value);

/**
 * Retrieve a message from the destination message queue matching the given request.
 * <ul>
 *  <li>if sender == NULL: get a message from any sender.</li>
 *  <li>if sender != NULL &amp;&amp; *sender == NULL: get a message from any sender and store the address of the sending tcb in *sender.</li>
 *  <li>if sender != NULL &amp;&amp *sender != NULL: get a message sent by *sender.</li>
 * </ul>
 * @param sender tcb_t**: (more info in the function description).
 * @param destination tcb_t*: pointer to destination.
 * @param value uintptr_t*: pointer to the area where the payload has to be saved.
 * @return int: -1 if there is no message matching the request, 0 otherwise.
 */
int msgq_get(struct tcb_t** sender, struct tcb_t* destination, uintptr_t* value);

#endif
