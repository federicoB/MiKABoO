#ifndef SSI_H
#define SSI_H

/**
 * System Service Interface entry point.
 */
void SSI_entry_point();

/* delete a process, its children and threads*/
void proc_terminate(struct pcb_t* proc);

#endif /* SSI_H */

