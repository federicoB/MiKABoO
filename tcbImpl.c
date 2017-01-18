#include "mikabooq.h";
#include "listx.h";
#include "const.h";

struct tcb_t tcbArray [MAXTHREAD];
struct list_head freeTCB;

struct tcb_t* extractTCB();

void thread_init(){
    if (MAXTHREAD == 0) exit();
    INIT_LIST_HEAD(&freeTCB);
    for (int i = 0; < MAXTHREAD; i++){
        list_add(&(tcbArray[i].t_next), &freeTCB);
    }
}

struct tcb_t *thread_alloc(struct pcb_t *process){
    struct tcb_t* newTCB = NULL;
    if(process !=NULL && !list_empty(&freeTCB)){
        newTCB = extractTCB();
        INIT_LIST_HEAD(&(newTCB->t_msgq));
        list_add(&(newTCB->t_next), &(process->p_threads));
        newTCB->t_pcb = process;
    }
    return newTCB;
}

int thread_free(struct tcb_t * oldthread){
    int result = -1;
    if(list_empty(&(oldthread->t_msgq))){
        list_del(&(oldthread->t_next));
        list_add(&(oldthread->t_next), &(freeTCB));
        result = 0;
    }
    return result;
}

struct tcb_t* extractTCB(){
    struct tcb_t* extractedTCB = container_of(list_next(&freeTCB), tcb_t, t_next);
    list_del(&(extractedTCB->t_next));
    return extractedTCB;
}

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
