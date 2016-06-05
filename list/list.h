#ifndef LIST_H
#define LIST_H

#include <stddef.h>

typedef struct List List;
typedef struct ListIter ListIter;

List *listCreate(void *(*alloc)(size_t), void (*dealloc)(void *));
void listSetDupMethod(List *list, void *(*dup)(void *));
void listSetFreeMethod(List *list, void (*free)(void *));
void listSetCompareMethod(List *list, int (*compare)(void *, void *));
void *(*listGetDupMethod(List *list))(void *);
void (*listGetFreeMethod(List *list))(void *);
int (*listGetCompareMethod(List *list))(void *, void *);
size_t listLength(List *list);
void listPushHead(List *list, void *value);
void listPushTail(List *list, void *value);
void listInsert(List *list, int index, void *value);
int listContains(List *list, void *value);
void *listIndex(List *list, int index);
void *listPopHead(List *list);
void *listPopTail(List *list);
void *listRemove(List *list, int index);
void listDel(List *list, void *value);
List *listDup(List *list);
void listRotate(List *list);
void listClear(List *list);
void listDestroy(List *list);
ListIter *listIterator(List *list);
ListIter *listReverseIterator(List *list);

int listIterHasNext(ListIter *iter);
void *listIterNext(ListIter *iter);
void listIterDestroy(ListIter *iter);

#endif
