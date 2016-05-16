#ifndef RBTREE_H
#define RBTREE_H

#include <stddef.h>

typedef struct RBTree RBTree;

RBTree *rbtreeCreate(void *(*alloc)(size_t), void (*dealloc)(void *));
size_t rbtreeSize(RBTree *tree);
int rbtreeContains(RBTree *tree, void *key);
void *rbtreeGet(RBTree *tree, void *key);
RBTree *rbtreeSet(RBTree *tree, void *key, void *value);
RBTree *rbtreeDel(RBTree *tree, void *key);
RBTree *rbtreeClear(RBTree *tree);
void rbtreeDestroy(RBTree *tree);

#endif
