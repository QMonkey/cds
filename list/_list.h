#ifndef _LIST_H
#define _LIST_H

#include <stddef.h>

typedef struct _ListNode {
	struct _ListNode *prev;
	struct _ListNode *next;
} _ListNode;

typedef struct _List {
	_ListNode *head;
	_ListNode *tail;
	size_t length;
} _List;

#define _listNodePrev(n) ((n)->prev)
#define _listNodeNext(n) ((n)->next)

#define _listLength(l) ((l)->length)
#define _listFirst(l) ((l)->head)
#define _listLast(l) ((l)->tail)

_List *_listCreate();
_List *_listAddHead(_List *list, _ListNode *node);
_List *_listAddTail(_List *list, _ListNode *node);
_List *_listInsert(_List *list, _ListNode *node, _ListNode *newNode, int after);
_ListNode *_listIndex(_List *list, int index);
void _listRemove(_List *list, _ListNode *node);
void _listRotate(_List *list);

#endif
