/*****************************************************************************
 * p1test.c Year 2017 v.0.1 December, 20 2016                                *
 * Copyright 2016 Renzo Davoli                                               *
 *                                                                           *
 * This file is part of MiKABoO.                                             *
 *                                                                           *
 * MiKABoO is free software; you can redistribute it and/or modify it under  *
 * the terms of the GNU General Public License as published by the Free      *
 * Software Foundation; either version 2 of the License, or (at your option) *
 * any later version.                                                        *
 * This program is distributed in the hope that it will be useful, but       *
 * WITHOUT ANY WARRANTY; without even the implied warranty of                *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General *
 * Public License for more details.                                          *
 * You should have received a copy of the GNU General Public License along   *
 * with this program; if not, write to the Free Software Foundation, Inc.,   *
 * 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.                  *
 *****************************************************************************/

#include <uARMconst.h>
#include <uARMtypes.h>
#include <libuarm.h>

#include "const.h"
#include "listx.h"
#include "mikabooq.h"
#include <stdint.h>

void addokbuf(char *strp) {
	  tprint(strp);
}

void adderrbuf(char *strp) {
	tprint(strp);
	PANIC();
}

void test_proc_alloc(struct pcb_t *root) {
	int i;
	struct pcb_t *pcb;
	/* test alloc dealloc of processes */
	for(i = 0; i < MAXPROC - 1; i++) {
		if (proc_alloc(root) == NULL)
			adderrbuf("ERROR proc_alloc: wrongly unavailable pcb\n");
	}
	if (proc_alloc(root) != NULL)
		adderrbuf("ERROR proc_alloc: wrongly available pcb\n");
	addokbuf("proc_alloc: Okay\n");
	for(i = 0; i < MAXPROC - 1; i++) {
		if (list_empty(&root->p_children))
			adderrbuf("ERROR wrong management of children list");
		pcb = proc_firstchild(root);
		proc_delete(pcb);
	}
	if (!list_empty(&root->p_children))
		adderrbuf("ERROR wrong management of children list (extra items)");
	addokbuf("proc_delete: Okay\n");
}


void test_proc_hierarchy(struct pcb_t *root) {
	int i;
	struct pcb_t *pcb;
	/* test hierarchy */
	for(i = 0; i < 3; i++) {
		if (proc_alloc(root) == NULL)
			adderrbuf("ERROR proc_alloc: wrongly unavailable pcb\n");
	}
	list_for_each_entry(pcb, &root->p_children, p_siblings) {
		for (i = 0; i < 4; i++) {
			if (proc_alloc(pcb) == NULL)
				adderrbuf("ERROR proc_alloc: wrongly unavailable pcb\n");
		}
	}
	pcb = proc_firstchild(root);
	if (proc_delete(pcb) != -1)
		adderrbuf("ERROR proc_delete should fail if non-empty p_children \n");
	i=0;
	while (!list_empty(&root->p_children)) {
		for (pcb = root; !list_empty(&pcb->p_children); 
				pcb = proc_firstchild(pcb))
			;
		if (proc_delete(pcb) == -1)
			adderrbuf("ERROR proc_delete should succeed if empty p_children \n");
		i++;
	}
	if (i != 15)
		adderrbuf("ERROR wrong # of elements in hierarchy\n");
	addokbuf("proc hierarchy: Okay\n");
}

void test_thread_alloc(struct pcb_t *root) {
	int i;
	struct tcb_t *tcb;
	/* test thread alloc dealloc */
	if (thread_alloc(NULL) != NULL)
		adderrbuf("thread_alloc should fail if process == NULL\n");
  for(i = 0; i < MAXTHREAD; i++) {
		if (thread_alloc(root) == NULL)
			adderrbuf("ERROR thread_alloc: wrongly unavailable tcb\n");
	}
	if (thread_alloc(root) != NULL)
		adderrbuf("ERROR thread_alloc: wrongly available tcb\n");
	addokbuf("thread_alloc: Okay\n");
	for(i = 0; i < MAXTHREAD; i++) {
		if (list_empty(&root->p_threads))
			adderrbuf("ERROR wrong management of thread list");
		tcb = proc_firstthread(root);
		thread_free(tcb);
	}
	if (!list_empty(&root->p_threads))
		adderrbuf("ERROR wrong management of thread list (extra items)");
	addokbuf("thread_delete: Okay\n");
}

void test_thread_queue(struct pcb_t *root) {
	struct tcb_t *tcb[20];
	int i;
	LIST_HEAD(tcbq);
	LIST_HEAD(tcbqq);
	if (thread_qhead(&tcbq) != NULL)
		adderrbuf("ERROR thread_qhead returns non NULL on an empty queue\n");
	if (thread_dequeue(&tcbqq) != NULL)
		adderrbuf("ERROR thread_dequeue returns non NULL on an empty queue\n");

	for (i = 0; i < 20; i++) {
		tcb[i] = thread_alloc(root);
		thread_enqueue(tcb[i], &tcbq);
	}
	for (i = 0; i < 20; i++) {
		struct tcb_t *ttcb = thread_dequeue(&tcbq);
		if (ttcb == NULL)
			adderrbuf("ERROR thread_dequeue missing elements \n");
		if (ttcb != tcb[i])
			adderrbuf("ERROR thread_enqueue FIFO mismatch\n");
	}
	for (i = 0; i < 20; i++)
		thread_enqueue(tcb[i], i & 1 ? &tcbqq : &tcbq);
	if (thread_qhead(&tcbq) != tcb[0] ||
			thread_qhead(&tcbqq) != tcb[1])
		adderrbuf("ERROR thread_qhead returns the wrong element\n");
	for (i = 0; i < 10; i++) {
		struct tcb_t *ttcb = thread_dequeue(&tcbq);
		if (ttcb == NULL || ttcb != tcb[2 * i])
			adderrbuf("ERROR thread_enqueue FIFO mismatch (multiq)\n");
	}
	for (i = 0; i < 10; i++) {
		struct tcb_t *ttcb = thread_dequeue(&tcbqq);
		if (ttcb == NULL || ttcb != tcb[2 * i + 1])
			adderrbuf("ERROR thread_enqueue FIFO mismatch (multiq)\n");
	}
	for (i = 0; i < 20; i++) 
		thread_free(tcb[i]);
	addokbuf("thread queue: Okay\n");
}

void test_msg_queue(struct pcb_t *root) {
	uintptr_t value;
	struct tcb_t *alpha, *bravo, *charlie;
	struct tcb_t *sender;
	alpha = thread_alloc(root);
	bravo = thread_alloc(root);
	charlie = thread_alloc(root);
	
	for (value = 0; value < MAXMSG; value++) {
		if (msgq_add(alpha, bravo, value) != 0)
			adderrbuf("ERROR msgq_add wrongly unavailable msg\n");
	}
	if (msgq_add(alpha, bravo, value) != -1)
		adderrbuf("ERROR msgq_add wrongly available msg\n");
	for (value = 0; value < MAXMSG; value++) {
		uintptr_t new_value;
		if (msgq_get(NULL, bravo, &new_value) != 0)
			adderrbuf("ERROR msgq_get lost messages\n");
		if (new_value != value)
			adderrbuf("ERROR msgq_get  FIFO mismatch\n");
	}
	if (msgq_get(NULL, bravo, &value) != -1)
		adderrbuf("ERROR msgq_get extra messages\n");

	msgq_add(alpha, bravo, 42);
	msgq_add(alpha, bravo, 48);
	msgq_add(alpha, bravo, 128);
	msgq_add(charlie, bravo, 2017);
	sender = NULL;
	msgq_get(&sender, bravo, &value);
	if (sender != alpha || value != 42)
		adderrbuf("ERROR msgq_get wrong sender or value\n");
	msgq_get(&charlie, bravo, &value);
	if (value != 2017)
		adderrbuf("ERROR msgq_get wrong element (sender search)\n");
	if (msgq_get(NULL, alpha, &value) == 0)
		adderrbuf("ERROR msgq_get wrong queue\n");
	while (msgq_get(NULL, bravo, &value) == 0)
		;
	thread_free(alpha);
	thread_free(bravo);
	thread_free(charlie);
	addokbuf("message queue: Okay\n");
}

int main(int argc, char* * argv[]) {
	int i;
	struct pcb_t *root = proc_init();
	struct pcb_t *pcb;
	struct tcb_t *tcb;
	/* initialize data structures */
	addokbuf("proc_init done\n");
	thread_init();
	addokbuf("thread_init done\n");
	msgq_init();
	addokbuf("msgq_init done\n");
	test_proc_alloc(root);
	test_proc_hierarchy(root);
	test_thread_alloc(root);
	test_thread_queue(root);
	test_msg_queue(root);
	addokbuf("That's one small step for a program, one giant leap for an operating system.\n");
	return 0;
}
