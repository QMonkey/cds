#ifndef RBTREE_H
#define RBTREE_H

#include "pair.h"

#include <stddef.h>

typedef struct RBTree RBTree;
typedef struct RBTreeIter RBTreeIter;

RBTree *rbtreeCreate(void *(*alloc)(size_t), void (*dealloc)(void *));
size_t rbtreeSize(RBTree *tree);
int rbtreeContains(RBTree *tree, void *key);
void *rbtreeGet(RBTree *tree, void *key);
RBTree *rbtreeSet(RBTree *tree, void *key, void *value);
RBTree *rbtreeDel(RBTree *tree, void *key);
RBTree *rbtreeClear(RBTree *tree);
void rbtreeDestroy(RBTree *tree);
RBTreeIter *rbtreeIterator(RBTree *tree);

int rbtreeIterHasNext(RBTreeIter *iter);
Pair rbtreeIterNext(RBTreeIter *iter);
void rbtreeIterDestroy(RBTreeIter *iter);

#endif
