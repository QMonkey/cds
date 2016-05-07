#ifndef LIST_H
#define LIST_H

#include <stddef.h>

typedef struct ListNode {
	struct ListNode *prev;
	struct ListNode *next;
} ListNode;

typedef struct List {
	ListNode *head;
	ListNode *tail;
	size_t length;
} List;

#define listNodePrev(n) ((n)->prev)
#define listNodeNext(n) ((n)->next)

#define listLength(l) ((l)->length)
#define listFirst(l) ((l)->head)
#define listLast(l) ((l)->tail)

List *listCreate();
List *listAddHead(List *list, ListNode *node);
List *listAddTail(List *list, ListNode *node);
List *listInsert(List *list, ListNode *node, ListNode *newNode, int after);
ListNode *listIndex(List *list, int index);
void listRemove(List *list, ListNode *node);
void listRotate(List *list);

#endif
