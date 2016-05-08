#include "base.h"
#include "clist.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct CListNode {
	void *value;
	ListNode node;
} CListNode;

#define clistNodeGet(n)                                                        \
	((n) != NULL ? container_of((n), CListNode, node) : NULL)

static CListNode *clistNodePrev(CListNode *node)
{
	return clistNodeGet(node->node.prev);
}

static CListNode *clistNodeNext(CListNode *node)
{
	return clistNodeGet(node->node.next);
}

CList *clistCreate()
{
	CList *list = malloc(sizeof(CList));
	if (list == NULL) {
		return NULL;
	}

	memset(list, 0, sizeof(CList));
	return list;
}

CList *clistPushHead(CList *list, void *value)
{
	CListNode *node = malloc(sizeof(CListNode));
	node->value = value;
	listAddHead(&list->list, &node->node);
	return list;
}

CList *clistPushTail(CList *list, void *value)
{
	CListNode *node = malloc(sizeof(CListNode));
	node->value = value;
	listAddTail(&list->list, &node->node);
	return list;
}

CList *clistInsert(CList *list, int index, void *value)
{
	assert(index >= 0 && index <= clistLength(list));

	if (index == 0) {
		return clistPushHead(list, value);
	}

	if (index == clistLength(list)) {
		return clistPushTail(list, value);
	}

	CListNode *newNode = malloc(sizeof(CListNode));
	newNode->value = value;

	int i;
	CListNode *node = clistNodeGet(list->list.head);
	for (i = 0; i < clistLength(list); ++i) {
		if (i == index) {
			break;
		}

		node = clistNodeNext(node);
	}

	if (node == NULL) {
		return list;
	}

	listInsert(&list->list, &node->node, &newNode->node, 0);

	return list;
}

int clistContains(CList *list, void *value)
{
	CListIter *iter = clistIterator(list);
	while (clistIterHasNext(iter)) {
		if (list->compare(clistIterNext(iter), value) == 0) {
			return 1;
		}
	}

	return 0;
}

void *clistIndex(CList *list, int index)
{
	ListNode *node = listIndex(&list->list, index);
	CListNode *cnode = clistNodeGet(node);
	return cnode->value;
}

CListIter *clistIterator(CList *list)
{
	CListIter *iter = malloc(sizeof(CListIter));
	iter->direction = DIRECTION_ASCENDING;
	iter->next = listFirst(&list->list);
	return iter;
}

CListIter *clistReverseIterator(CList *list)
{
	CListIter *iter = malloc(sizeof(CListIter));
	iter->direction = DIRECTION_DESCENDING;
	iter->next = listLast(&list->list);
	return iter;
}

void *clistPopHead(CList *list) { return clistRemove(list, 0); }

void *clistPopTail(CList *list)
{
	CListNode *node = clistNodeGet(list->list.tail);
	if (node == NULL) {
		return NULL;
	}

	listRemove(&list->list, &node->node);
	void *value = node->value;
	free(node);

	return value;
}

void *clistRemove(CList *list, int index)
{
	int i;
	CListNode *node = clistNodeGet(list->list.head);
	for (i = 0; i < clistLength(list); ++i) {
		if (i == index) {
			break;
		}

		node = clistNodeNext(node);
	}

	if (node == NULL) {
		return NULL;
	}

	listRemove(&list->list, &node->node);
	void *value = node->value;
	free(node);

	return value;
}

void clistDel(CList *list, void *value)
{
	CListNode *node = clistNodeGet(list->list.head);
	while (node != NULL) {
		if (list->compare(node->value, value) == 0) {
			break;
		}

		node = clistNodeNext(node);
	}

	if (node == NULL) {
		return;
	}

	listRemove(&list->list, &node->node);
	if (list->free != NULL) {
		list->free(node->value);
	}
	free(node);
}

CList *clistDup(CList *list)
{
	CList *l = clistCreate();
	l->free = list->free;
	l->dup = list->dup;
	l->compare = list->compare;

	CListNode *node = clistNodeGet(list->list.head);
	while (node != NULL) {
		clistPushTail(l, l->dup(node->value));
	}

	return l;
}

void clistRotate(CList *list) { listRotate(&list->list); }

int clistIterHasNext(CListIter *iter) { return iter->next != NULL; }

void *clistIterNext(CListIter *iter)
{
	CListNode *cnode = clistNodeGet(iter->next);

	if (iter->direction) {
		iter->next = listNodeNext(iter->next);
	} else {
		iter->next = listNodePrev(iter->next);
	}

	return cnode->value;
}

#undef clistNodeGet
