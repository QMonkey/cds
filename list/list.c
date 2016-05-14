#include "list.h"

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DIRECTION_ASCENDING 1
#define DIRECTION_DESCENDING 0

typedef struct ListNode {
	void *value;
	struct ListNode *prev;
	struct ListNode *next;
} ListNode;

struct List {
	void *(*alloc)(size_t);
	void (*dealloc)(void *);
	void *(*dup)(void *value);
	void (*free)(void *value);
	int (*compare)(void *value1, void *value2);

	size_t length;
	struct ListNode *head;
	struct ListNode *tail;
};

struct ListIter {
	int direction;
	void (*dealloc)(void *);
	struct ListNode *next;
};

List *listCreate(void *(*alloc)(size_t), void (*dealloc)(void *))
{
	List *list = alloc(sizeof(List));
	if (list == NULL) {
		return NULL;
	}

	memset(list, 0, sizeof(List));
	list->alloc = alloc;
	list->dealloc = dealloc;

	return list;
}

size_t listLength(List *list) { return list->length; }

List *listPushHead(List *list, void *value)
{
	ListNode *node = list->alloc(sizeof(ListNode));
	node->value = value;

	if (list->length != 0) {
		list->head->prev = node;
	} else {
		list->tail = node;
	}

	node->prev = NULL;
	node->next = list->head;
	list->head = node;

	++list->length;
	return list;
}

List *listPushTail(List *list, void *value)
{
	ListNode *node = list->alloc(sizeof(ListNode));
	node->value = value;

	if (list->length != 0) {
		list->tail->next = node;
	} else {
		list->head = node;
	}

	node->prev = list->tail;
	node->next = NULL;
	list->tail = node;

	++list->length;
	return list;
}

List *listInsert(List *list, int index, void *value)
{
	assert(index >= 0 && index <= list->length);

	if (index == 0) {
		return listPushHead(list, value);
	}

	if (index == list->length) {
		return listPushTail(list, value);
	}

	ListNode *newNode = list->alloc(sizeof(ListNode));
	newNode->value = value;

	int i;
	ListNode *node = list->head;
	for (i = 1; i < list->length; ++i) {
		if (i == index) {
			break;
		}

		node = node->next;
	}

	ListNode *prev = node->prev;
	newNode->prev = prev;
	newNode->next = node;
	if (prev != NULL) {
		prev->next = newNode;
	} else {
		list->head = newNode;
	}
	node->prev = newNode;

	++list->length;
	return list;
}

int listContains(List *list, void *value)
{
	ListNode *node = list->head;
	while (node != NULL) {
		if (list->compare(node->value, value) == 0) {
			return 1;
		}
	}

	return 0;
}

void *listIndex(List *list, int index)
{
	if (index < 0 || index >= list->length) {
		return NULL;
	}

	int i;
	ListNode *node = list->head;
	for (i = 0; i < list->length; ++i) {
		if (i == index) {
			break;
		}

		node = node->next;
	}

	return node;
}

static void _listRemove(List *list, ListNode *node)
{
	if (node == NULL) {
		return;
	}

	ListNode *prev = node->prev;
	ListNode *next = node->next;

	if (list->head == node) {
		list->head = next;
	}

	if (list->tail == node) {
		list->tail = prev;
	}

	if (prev != NULL) {
		prev->next = next;
	}

	if (next != NULL) {
		next->prev = prev;
	}

	--list->length;
}

void *listPopHead(List *list) { return listRemove(list, 0); }

void *listPopTail(List *list)
{
	ListNode *node = list->tail;
	_listRemove(list, node);
	void *value = node->value;
	list->dealloc(node);

	return value;
}

void *listRemove(List *list, int index)
{
	assert(index >= 0 && index < list->length);

	int i;
	ListNode *node = list->head;
	for (i = 0; i < list->length; ++i) {
		if (i == index) {
			break;
		}

		node = node->next;
	}

	_listRemove(list, node);
	void *value = node->value;
	list->dealloc(node);

	return value;
}

void listDel(List *list, void *value)
{
	ListNode *node = list->head;
	while (node != NULL) {
		if (list->compare(node->value, value) == 0) {
			break;
		}

		node = node->next;
	}

	if (node == NULL) {
		return;
	}

	_listRemove(list, node);

	if (list->free != NULL) {
		list->free(node->value);
	}

	list->dealloc(node);
}

List *listDup(List *list)
{
	List *l = listCreate(list->alloc, list->dealloc);
	l->free = list->free;
	l->dup = list->dup;
	l->compare = list->compare;

	ListNode *node = list->head;
	while (node != NULL) {
		listPushTail(l, list->dup(node->value));
		node = node->next;
	}

	return l;
}

void listRotate(List *list)
{
	if (list->length == 0) {
		return;
	}

	ListNode *current = list->head;
	list->head = list->tail;
	list->tail = current;

	ListNode *next = NULL;
	while (current != NULL) {
		next = current->next;
		current->next = current->prev;
		current->prev = next;
		current = next;
	}
}

ListIter *listIterator(List *list)
{
	ListIter *iter = list->alloc(sizeof(ListIter));
	iter->direction = DIRECTION_ASCENDING;
	iter->next = list->head;
	iter->dealloc = list->dealloc;
	return iter;
}

ListIter *listReverseIterator(List *list)
{
	ListIter *iter = list->alloc(sizeof(ListIter));
	iter->direction = DIRECTION_DESCENDING;
	iter->next = list->tail;
	iter->dealloc = list->dealloc;
	return iter;
}

int listIterHasNext(ListIter *iter) { return iter->next != NULL; }

void *listIterNext(ListIter *iter)
{
	void *value = iter->next->value;

	if (iter->direction) {
		iter->next = iter->next->next;
	} else {
		iter->next = iter->next->prev;
	}

	return value;
}

void listIterDestroy(ListIter *iter) { iter->dealloc(iter); }
