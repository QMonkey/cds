#ifndef CLIST_H
#define CLIST_H

#include "list.h"

#include <stddef.h>

#define DIRECTION_ASCENDING 1
#define DIRECTION_DESCENDING 0

typedef struct CListIter {
	int direction;
	ListNode *next;
} CListIter;

typedef struct CList {
	void *(*dup)(void *value);
	void (*free)(void *value);
	int (*compare)(void *value1, void *value2);
	List list;
} CList;

#define clistLength(l) (listLength(&(l)->list))

#define clistSetDupMethod(l, m) ((l)->dup = (m))
#define clistSetFreeMethod(l, m) ((l)->free = (m))
#define clistSetCompareMethod(l, m) ((l)->compare = (m))

#define clistGetDupMethod(l) ((l)->dup)
#define clistGetFreeMethod(l) ((l)->free)
#define clistGetCompareMethod(l) ((l)->compare)

CList *clistCreate();
CList *clistPushHead(CList *list, void *value);
CList *clistPushTail(CList *list, void *value);
CList *clistInsert(CList *list, int index, void *value);
int clistContains(CList *list, void *value);
void *clistIndex(CList *list, int index);
CListIter *clistIterator(CList *list);
CListIter *clistReverseIterator(CList *list);
void *clistPopHead(CList *list);
void *clistPopTail(CList *list);
void *clistRemove(CList *list, int index);
void clistDel(CList *list, void *value);
CList *clistDup(CList *list);
void clistRotate(CList *list);

int clistIterHasNext(CListIter *iter);
void *clistIterNext(CListIter *iter);

#endif
