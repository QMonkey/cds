#include "base.h"
#include "list.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct ListNode {
	void *value;
	_ListNode node;
} ListNode;

#define listNodeGet(n) ((n) != NULL ? container_of((n), ListNode, node) : NULL)

static ListNode *listNodeNext(ListNode *node)
{
	return listNodeGet(node->node.next);
}

List *listCreate()
{
	List *list = malloc(sizeof(List));
	if (list == NULL) {
		return NULL;
	}

	memset(list, 0, sizeof(List));
	return list;
}

List *listPushHead(List *list, void *value)
{
	ListNode *node = malloc(sizeof(ListNode));
	node->value = value;
	_listAddHead(&list->list, &node->node);
	return list;
}

List *listPushTail(List *list, void *value)
{
	ListNode *node = malloc(sizeof(ListNode));
	node->value = value;
	_listAddTail(&list->list, &node->node);
	return list;
}

List *listInsert(List *list, int index, void *value)
{
	assert(index >= 0 && index <= listLength(list));

	if (index == 0) {
		return listPushHead(list, value);
	}

	if (index == listLength(list)) {
		return listPushTail(list, value);
	}

	ListNode *newNode = malloc(sizeof(ListNode));
	newNode->value = value;

	int i;
	ListNode *node = listNodeGet(list->list.head);
	for (i = 0; i < listLength(list); ++i) {
		if (i == index) {
			break;
		}

		node = listNodeNext(node);
	}

	if (node == NULL) {
		return list;
	}

	_listInsert(&list->list, &node->node, &newNode->node, 0);

	return list;
}

int listContains(List *list, void *value)
{
	ListIter *iter = listIterator(list);
	while (listIterHasNext(iter)) {
		if (list->compare(listIterNext(iter), value) == 0) {
			return 1;
		}
	}

	return 0;
}

void *listIndex(List *list, int index)
{
	_ListNode *node = _listIndex(&list->list, index);
	ListNode *cnode = listNodeGet(node);
	return cnode->value;
}

ListIter *listIterator(List *list)
{
	ListIter *iter = malloc(sizeof(ListIter));
	iter->direction = DIRECTION_ASCENDING;
	iter->next = _listFirst(&list->list);
	return iter;
}

ListIter *listReverseIterator(List *list)
{
	ListIter *iter = malloc(sizeof(ListIter));
	iter->direction = DIRECTION_DESCENDING;
	iter->next = _listLast(&list->list);
	return iter;
}

void *listPopHead(List *list) { return listRemove(list, 0); }

void *listPopTail(List *list)
{
	ListNode *node = listNodeGet(list->list.tail);
	if (node == NULL) {
		return NULL;
	}

	_listRemove(&list->list, &node->node);
	void *value = node->value;
	free(node);

	return value;
}

void *listRemove(List *list, int index)
{
	int i;
	ListNode *node = listNodeGet(list->list.head);
	for (i = 0; i < listLength(list); ++i) {
		if (i == index) {
			break;
		}

		node = listNodeNext(node);
	}

	if (node == NULL) {
		return NULL;
	}

	_listRemove(&list->list, &node->node);
	void *value = node->value;
	free(node);

	return value;
}

void listDel(List *list, void *value)
{
	ListNode *node = listNodeGet(list->list.head);
	while (node != NULL) {
		if (list->compare(node->value, value) == 0) {
			break;
		}

		node = listNodeNext(node);
	}

	if (node == NULL) {
		return;
	}

	_listRemove(&list->list, &node->node);
	if (list->free != NULL) {
		list->free(node->value);
	}
	free(node);
}

List *listDup(List *list)
{
	List *l = listCreate();
	l->free = list->free;
	l->dup = list->dup;
	l->compare = list->compare;

	ListNode *node = listNodeGet(list->list.head);
	while (node != NULL) {
		listPushTail(l, l->dup(node->value));
	}

	return l;
}

void listRotate(List *list) { _listRotate(&list->list); }

int listIterHasNext(ListIter *iter) { return iter->next != NULL; }

void *listIterNext(ListIter *iter)
{
	ListNode *cnode = listNodeGet(iter->next);

	if (iter->direction) {
		iter->next = _listNodeNext(iter->next);
	} else {
		iter->next = _listNodePrev(iter->next);
	}

	return cnode->value;
}

#undef listNodeGet
