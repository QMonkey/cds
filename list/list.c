#include "list.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

List *listCreate()
{
	List *list = malloc(sizeof(List));
	if (list == NULL) {
		return NULL;
	}

	memset(list, 0, sizeof(List));
	return list;
}

List *listAddHead(List *list, void *value)
{
	ListNode *node = malloc(sizeof(ListNode));
	node->value = value;
	node->prev = NULL;
	node->next = list->head;
	list->head = node;
	++list->length;
	return list;
}

List *listAddTail(List *list, void *value)
{
	ListNode *node = malloc(sizeof(ListNode));
	node->value = value;
	node->next = NULL;
	node->prev = list->tail;
	list->tail = node;
	++list->length;
	return list;
}

List *listInsert(List *list, ListNode *node, void *value, int after)
{
	ListNode *newNode = malloc(sizeof(ListNode));
	newNode->value = value;

	if (after) {
		ListNode *next = node->next;
		if (next == NULL) {
			return listAddTail(list, value);
		}

		newNode->prev = node;
		newNode->next = next;
		node->next = newNode;
		next->prev = newNode;
	} else {
		ListNode *prev = node->prev;
		if (prev == NULL) {
			return listAddHead(list, value);
		}

		newNode->prev = prev;
		newNode->next = node;
		prev->next = newNode;
		node->prev = newNode;
	}

	++list->length;
	return list;
}

ListNode *listSearch(List *list, void *value)
{
	ListNode *node = list->head;
	while (node != NULL) {
		if (list->compare(node->value, value) == 0) {
			break;
		}

		node = node->next;
	}
	return node;
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

void listDel(List *list, ListNode *node)
{
	ListNode *prev = node->prev;
	ListNode *next = node->next;

	if (prev != NULL) {
		prev->next = next;
	}

	if (next != NULL) {
		next->prev = prev;
	}

	list->free(node->value);
	free(node);
}

void listRotate(List *list)
{
	if (listLength(list) == 0) {
		return;
	}

	ListNode *current = list->head;
	list->head = list->tail;
	list->tail = list->head;

	ListNode *next = NULL;
	while (current != NULL) {
		next = current->next;
		current->next = current->prev;
		current->prev = next;
		current = next;
	}
}
