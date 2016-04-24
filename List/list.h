#ifndef LIST_H
#define LIST_H

typedef struct ListNode {
	struct ListNode *prev;
	struct ListNode *next;
	void *value;
} ListNode;

typedef struct List {
	ListNode *head;
	ListNode *tail;
	size_t length;

	void *(*dup)(void *value);
	void (*free)(void *value);
	int (*compare)(void *value1, void *value2);
} List;

#define listNodeValue(n) ((n)->value)
#define listNodePrev(n) ((n)->prev)
#define listNodeNext(n) ((n)->next)

#define listLength(l) ((l)->length)
#define listFirst(l) ((l)->head)
#define listLast(l) ((l)->tail)

#endif
