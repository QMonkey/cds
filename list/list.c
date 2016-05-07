#include "list.h"

#include <stdio.h>
#include <string.h>

List *listAddHead(List *list, ListNode *node)
{
	if (list->head != NULL) {
		list->head->prev = node;
	}

	node->prev = NULL;
	node->next = list->head;
	list->head = node;
	++list->length;
	return list;
}

List *listAddTail(List *list, ListNode *node)
{
	if (list->tail != NULL) {
		list->tail->next = node;
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
		ListNode *next = node->next;
		if (next == NULL) {
			return listAddTail(list, newNode);
		}

		newNode->prev = node;
		newNode->next = next;
		node->next = newNode;
		next->prev = newNode;
	} else {
		ListNode *prev = node->prev;
		if (prev == NULL) {
			return listAddHead(list, newNode);
		}

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
