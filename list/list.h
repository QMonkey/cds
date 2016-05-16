#ifndef LIST_H
#define LIST_H

#include <stddef.h>

typedef struct List List;
typedef struct ListIter ListIter;

List *listCreate(void *(*alloc)(size_t), void (*dealloc)(void *));
List *listSetDupMethod(List *list, void *(*dup)(void *));
List *listSetFreeMethod(List *list, void (*free)(void *));
List *listSetCompareMethod(List *list, int (*compare)(void *, void *));
void *(*listGetDupMethod(List *list))(void *);
void (*listGetFreeMethod(List *list))(void *);
int (*listGetCompareMethod(List *list))(void *, void *);
size_t listLength(List *list);
List *listPushHead(List *list, void *value);
List *listPushTail(List *list, void *value);
List *listInsert(List *list, int index, void *value);
int listContains(List *list, void *value);
void *listIndex(List *list, int index);
void *listPopHead(List *list);
void *listPopTail(List *list);
void *listRemove(List *list, int index);
void listDel(List *list, void *value);
List *listDup(List *list);
void listRotate(List *list);
List *listClear(List *list);
void listDestroy(List *list);
ListIter *listIterator(List *list);
ListIter *listReverseIterator(List *list);

int listIterHasNext(ListIter *iter);
void *listIterNext(ListIter *iter);
void listIterDestroy(ListIter *iter);

#endif
