#include "_list.h"

#include <stdio.h>
#include <string.h>

_List *_listAddHead(_List *list, _ListNode *node)
{
	if (_listLength(list) != 0) {
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

_List *_listAddTail(_List *list, _ListNode *node)
{
	if (_listLength(list) != 0) {
		list->tail->next = node;
	} else {
		list->head = node;
	}

	node->next = NULL;
	node->prev = list->tail;
	list->tail = node;
	++list->length;
	return list;
}

_List *_listInsert(_List *list, _ListNode *node, _ListNode *newNode, int after)
{
	if (after) {
		if (list->tail == node) {
			return _listAddTail(list, newNode);
		}

		_ListNode *next = node->next;
		newNode->prev = node;
		newNode->next = next;
		node->next = newNode;
		next->prev = newNode;
	} else {
		if (list->head == node) {
			return _listAddHead(list, newNode);
		}

		_ListNode *prev = node->prev;
		newNode->prev = prev;
		newNode->next = node;
		prev->next = newNode;
		node->prev = newNode;
	}

	++list->length;
	return list;
}

_ListNode *_listIndex(_List *list, int index)
{
	_ListNode *node = list->head;
	int i;
	for (i = 0; i < _listLength(list); ++i) {
		if (i == index) {
			break;
		}

		node = node->next;
	}

	return node;
}

void _listRemove(_List *list, _ListNode *node)
{
	_ListNode *prev = node->prev;
	_ListNode *next = node->next;

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

void _listRotate(_List *list)
{
	if (_listLength(list) == 0) {
		return;
	}

	_ListNode *current = list->head;
	list->head = list->tail;
	list->tail = current;

	_ListNode *next = NULL;
	while (current != NULL) {
		next = current->next;
		current->next = current->prev;
		current->prev = next;
		current = next;
	}
}
