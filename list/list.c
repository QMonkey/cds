#include "list.h"

#include <stdio.h>
#include <string.h>

List *listAddHead(List *list, ListNode *node)
{
	if (listLength(list) != 0) {
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

List *listAddTail(List *list, ListNode *node)
{
	if (listLength(list) != 0) {
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

List *listInsert(List *list, ListNode *node, ListNode *newNode, int after)
{
	if (after) {
		if (list->tail == node) {
			return listAddTail(list, newNode);
		}

		ListNode *next = node->next;
		newNode->prev = node;
		newNode->next = next;
		node->next = newNode;
		next->prev = newNode;
	} else {
		if (list->head == node) {
			return listAddHead(list, newNode);
		}

		ListNode *prev = node->prev;
		newNode->prev = prev;
		newNode->next = node;
		prev->next = newNode;
		node->prev = newNode;
	}

	++list->length;
	return list;
}

ListNode *listIndex(List *list, int index)
{
	ListNode *node = list->head;
	int i;
	for (i = 0; i < listLength(list); ++i) {
		if (i == index) {
			break;
		}

		node = node->next;
	}

	return node;
}

void listRemove(List *list, ListNode *node)
{
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

void listRotate(List *list)
{
	if (listLength(list) == 0) {
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
