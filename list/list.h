#ifndef LIST_H
#define LIST_H

typedef struct List List;
typedef struct ListIter ListIter;

#define listSetDupMethod(l, m) ((l)->dup = (m))
#define listSetFreeMethod(l, m) ((l)->free = (m))
#define listSetCompareMethod(l, m) ((l)->compare = (m))

#define listGetDupMethod(l) ((l)->dup)
#define listGetFreeMethod(l) ((l)->free)
#define listGetCompareMethod(l) ((l)->compare)

List *listCreate();
List *listPushHead(List *list, void *value);
List *listPushTail(List *list, void *value);
List *listInsert(List *list, int index, void *value);
int listContains(List *list, void *value);
void *listIndex(List *list, int index);
ListIter *listIterator(List *list);
ListIter *listReverseIterator(List *list);
void *listPopHead(List *list);
void *listPopTail(List *list);
void *listRemove(List *list, int index);
void listDel(List *list, void *value);
List *listDup(List *list);
void listRotate(List *list);

int listIterHasNext(ListIter *iter);
void *listIterNext(ListIter *iter);

#endif
