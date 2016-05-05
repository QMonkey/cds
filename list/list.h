#ifndef LIST_H
#define LIST_H

#include <stddef.h>

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

#define listSetDupMethod(l, m) ((l)->dup = (m))
#define listSetFreeMethod(l, m) ((l)->free = (m))
#define listSetCompareMethod(l, m) ((l)->compare = (m))

#define listGetDupMethod(l) ((l)->dup)
#define listGetFreeMethod(l) ((l)->free)
#define listGetCompareMethod(l) ((l)->compare)

List *listCreate();
List *listAddHead(List *list, void *value);
List *listAddTail(List *list, void *value);
List *listInsert(List *list, ListNode *node, void *value, int after);
ListNode *listSearch(List *list, void *value);
ListNode *listIndex(List *list, int index);
void listDel(List *list, ListNode *node);
void listRotate(List *list);

#endif
