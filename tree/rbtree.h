#ifndef RBTREE_H
#define RBTREE_H

#include "pair.h"

#include <stddef.h>

typedef struct RBTree RBTree;
typedef struct RBTreeIter RBTreeIter;

RBTree *rbtreeCreate(void *(*alloc)(size_t), void (*dealloc)(void *));
void (*rbtreeGetFreeKeyMethod(RBTree *tree))(void *key);
void rbtreeSetFreeKeyMethod(RBTree *tree, void (*free_key)(void *));
void (*rbtreeGetFreeValueMethod(RBTree *tree))(void *value);
void rbtreeSetFreeValueMethod(RBTree *tree, void (*free_value)(void *));
int (*rbtreeGetCompareMethod(RBTree *tree))(void *key1, void *key2);
void rbtreeSetCompareMethod(RBTree *tree, int (*compare)(void *, void *));
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
