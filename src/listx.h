/* subset of the Linux Kernel source file: "include/linux/list.h"
GPLv2 */
#ifndef _LISTX_H
#define _LISTX_H

#ifndef NULL
#define NULL 0
#endif
typedef unsigned int    size_tt;

#define container_of(ptr, type, member) ({			\
		const typeof( ((type *)0)->member ) *__mptr = (ptr);	\
		(type *)( (char *)__mptr - offsetof(type,member) );})

#define offsetof(TYPE, MEMBER) ((size_tt) &((TYPE *)0)->MEMBER)

struct list_head {
	struct list_head *next, *prev;
};

#define LIST_HEAD_INIT(name) { &(name), &(name) }

#define LIST_HEAD(name) \
	struct list_head name = LIST_HEAD_INIT(name)

static inline void INIT_LIST_HEAD(struct list_head *list)
{
	list->next = list;
	list->prev = list;
}
static inline void __list_add(struct list_head *new,
		struct list_head *prev,
		struct list_head *next)
{
	next->prev = new;
	new->next = next;
	new->prev = prev;
	prev->next = new;
}
static inline void list_add(struct list_head *new, struct list_head *head)
{
	__list_add(new, head, head->next);
}
static inline void list_add_tail(struct list_head *new, struct list_head *head)
{
	__list_add(new, head->prev, head);
}
static inline void __list_del(struct list_head * prev, struct list_head * next)
{
	next->prev = prev;
	prev->next = next;
}
static inline void list_del(struct list_head *entry)
{
	__list_del(entry->prev, entry->next);
}
static inline int list_is_last(const struct list_head *list,
		const struct list_head *head)
{
	return list->next == head;
}
static inline int list_empty(const struct list_head *head)
{
	return head->next == head;
}
static inline struct list_head *list_next(const struct list_head *current)
{
	if (list_empty(current))
		return NULL;
	else
		return current->next;
}
static inline struct list_head *list_prev(const struct list_head *current)
{
	if (list_empty(current))
		return NULL;
	else
		return current->prev;
}

#define list_for_each(pos, head) \
	for (pos = (head)->next; pos != (head); pos = pos->next)

#define list_for_each_prev(pos, head) \
	for (pos = (head)->prev; pos != (head); pos = pos->prev)

#define list_for_each_entry(pos, head, member)                          \
	for (pos = container_of((head)->next, typeof(*pos), member);      \
	&pos->member != (head);        \
	pos = container_of(pos->member.next, typeof(*pos), member))

#define list_for_each_entry_reverse(pos, head, member)                  \
	for (pos = container_of((head)->prev, typeof(*pos), member);      \
	&pos->member != (head);        \
	pos = container_of(pos->member.prev, typeof(*pos), member))

#define list_for_each_entry_safe(pos, n, head, member)			\
	for (pos = container_of((head)->next, typeof(*pos), member),	\
	n = container_of((pos)->member.next, typeof(*pos), member);			\
	&pos->member != (head); 					\
	pos = n, n = container_of((n)->member.next, typeof(*pos), member))
#endif
