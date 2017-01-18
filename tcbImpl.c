#include "mikabooq.h"

void thread_enqueue(struct tcb_t *new, struct list_head *queue) {
    list_add_tail(&(new->t_sched),queue);
}

struct tcb_t *thread_qhead(struct list_head *queue) {
    struct tcb_t * result = NULL;
    if (!list_empty(queue)) {
        result = container_of(list_next(queue),tcb_t,t_sched);
    }
    return result;
}

struct tcb_t *thread_dequeue(struct list_head *queue) {
    struct tcb_t* result = NULL;
    if (!list_empty(queue)) {
        struct list_head * first = list_next(queue);
        result = container_of(first,tcb_t,t_sched);
        list_del(first);
    }
    return result;
}
