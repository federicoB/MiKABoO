#include <libuarm.h>
#include "mikabooq.h"
#include "const.h"
#include "listx.h"

/* extracts a pcb from the free pcb list. Must check size first. */
struct pcb_t* extractPCB();

//the array of pcbs
struct pcb_t pcbArray[MAXPROC];
//pcb free list entry point
struct list_head freePCB;

struct pcb_t* proc_init(){
    //if invalid const
    if(MAXPROC <= 0) {
        //print error message and panic
        tprint("INVALID MAXPROC\n");
        PANIC();
    }
    //init the free list
    INIT_LIST_HEAD(&freePCB);
    //declare index
    int i;
    //for the whole array of pcbs
    for (i = 0; i < MAXPROC; i++) {
        //add the pcb to the free list
        list_add(&(pcbArray[i].p_siblings), &freePCB);
    }
    //extract the a pcb from the free list
    struct pcb_t* rootPCB = extractPCB();
    //set root's parent as NULL
    rootPCB->p_parent = NULL;
    //init children list
    INIT_LIST_HEAD(&(rootPCB->p_children));
    //init thread list
    INIT_LIST_HEAD(&(rootPCB->p_threads));
    //TODO: choose how to init sibilings of root process------------------------
    //return the pointer to the root pcb
    return rootPCB;
}

struct pcb_t* proc_alloc(struct pcb_t* p_parent){
    //the result
    struct pcb_t* newPCB = NULL;
    //if parent is not NULL and there is a free pcb
    if (p_parent != NULL && !list_empty(&freePCB)){
        //extract a pcb
        newPCB = extractPCB();
        //init children list in new pcb
        INIT_LIST_HEAD(&(newPCB->p_children));
        //init thread list in new pcb
        INIT_LIST_HEAD(&(newPCB->p_threads));
        //insert the new pcb in the children list of the parent
        list_add(&(newPCB->p_siblings), &(p_parent->p_children));
        //set the parent in the new pcb
        newPCB->p_parent = p_parent;
        //init prg manager as NULL
        newPCB->prgMgr = NULL;
        //init tlb manager as NULL
        newPCB->tlbMgr = NULL;
        //init sys/bk manager as NULL
        newPCB->sysMgr = NULL;
    }//else leave as NULL
    //return the result
    return newPCB;
}

int proc_delete(struct pcb_t* oldproc){
    //the result code (default: error)
    int result = -1;
    //if the process has no children or threads
    if(list_empty(&(oldproc->p_children)) && list_empty(&(oldproc->p_threads))){
        //remove oldproc from the parent's children list
        list_del(&(oldproc->p_siblings));
        //erase parent link (not necessary)
        oldproc->p_parent = NULL;
        //add oldproc to the free list
        list_add(&(oldproc->p_siblings), &freePCB);
        //set result as 0 (success)
        result = 0;
    }
    //return the result
    return result;
}

struct pcb_t* proc_firstchild(struct pcb_t* proc){
    //the first child
    struct pcb_t* firstChild = NULL;
    //if the list is not empty
    if(!list_empty(&(proc->p_children))){
        //extract a pcb from the first children list element
        firstChild = container_of(list_next(&(proc->p_children)),struct pcb_t, p_siblings);
    }
    //return the first child
    return firstChild;
}

struct tcb_t* proc_firstthread(struct pcb_t* proc){
    //the first thread
    struct tcb_t* firstThread = NULL;
    //if the list is not empty
    if(!list_empty(&(proc->p_threads))){
        //extract a tcb from the first thread list element
        firstThread = container_of(list_next(&(proc->p_threads)),struct tcb_t, t_next);
    }
    //return the first thread
    return firstThread;
}

struct pcb_t* extractPCB(){
    //extract a pcb from the first list element
    struct pcb_t* extractedPCB = container_of(list_next(&freePCB),struct pcb_t, p_siblings);
    //remove the extracted pcb from the free list
    list_del(&(extractedPCB->p_siblings));
    //return the pcb
    return extractedPCB;
}